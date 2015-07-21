//
//  JWT.mm
//  JWT
//
//  Created by Alexander Trishyn on 7/21/15.
//  Copyright (c) 2015 Lohika. The MIT License (MIT).
//

#import "JWT.h"
#import "JWTSignatureAlgorithm.h"
#import "JWTVersion.h"
#import "JWTBase64.h"

@implementation NSData(URLSafeBase64)

- (NSString*)urlSafeBase64String
{
    std::string value((const char *)[self bytes], [self length]);
    std::string result = jwt::Base64::encode(value);
    return @(result.c_str());
}

@end

@implementation JWTClaims

- (NSDate*)expirationDate;
{
    if (!_expirationDate)
        _expirationDate = [NSDate distantFuture];
    return _expirationDate;
}

- (NSDate*)notBeforeDate;
{
    if (!_notBeforeDate)
        _notBeforeDate = [NSDate distantPast];
    return _notBeforeDate;
}

- (NSDate*)issuedAt;
{
    if (!_issuedAt)
        _issuedAt = [NSDate date];
    return _issuedAt;
}

#pragma mark-

+ (NSDictionary*)dictionaryWithClaims:(JWTClaims*)claims
{
    NSMutableDictionary* dictionary = [NSMutableDictionary dictionary];
    [self dictionary:dictionary setObjectIfNotNil:claims.issuer forKey:@"iss"];
    [self dictionary:dictionary setObjectIfNotNil:claims.subject forKey:@"sub"];
    [self dictionary:dictionary setObjectIfNotNil:claims.audience forKey:@"aud"];
    [self dictionary:dictionary setObjectIfNotNil:@([claims.expirationDate timeIntervalSince1970]) forKey:@"exp"];
    [self dictionary:dictionary setObjectIfNotNil:@([claims.notBeforeDate timeIntervalSince1970]) forKey:@"nbf"];
    [self dictionary:dictionary setObjectIfNotNil:@([claims.issuedAt timeIntervalSince1970]) forKey:@"iat"];
    [self dictionary:dictionary setObjectIfNotNil:claims.identifier forKey:@"jti"];
    [self dictionary:dictionary setObjectIfNotNil:claims.type forKey:@"typ"];
    return dictionary;
}

+ (JWTClaims*)claimsWithDictionary:(NSDictionary*)dictionary
{
    JWTClaims* claims = [[JWTClaims alloc] init];
    claims.issuer = [dictionary objectForKey:@"iss"];
    claims.subject = [dictionary objectForKey:@"sub"];
    claims.audience = [dictionary objectForKey:@"aud"];
    claims.expirationDate = [NSDate dateWithTimeIntervalSince1970:[[dictionary objectForKey:@"exp"] doubleValue]];
    claims.notBeforeDate = [NSDate dateWithTimeIntervalSince1970:[[dictionary objectForKey:@"nbf"] doubleValue]];
    claims.issuedAt = [NSDate dateWithTimeIntervalSince1970:[[dictionary objectForKey:@"iat"] doubleValue]];
    claims.identifier = [dictionary objectForKey:@"jti"];
    claims.type = [dictionary objectForKey:@"typ"];
    return claims;
}

+ (void)dictionary:(NSMutableDictionary*)dictionary setObjectIfNotNil:(id)object forKey:(id<NSCopying>)key
{
    if (!object)
        return;
    [dictionary setObject:object forKey:key];
}

@end

#pragma mark-

@implementation JWT

+ (NSString*)version
{
    return module_version;
}

#pragma mark-

+ (NSString*)encodeClaims:(JWTClaims*)claims withSecret:(NSString*)secret
{
    return [self encodeClaims:claims withSecret:secret algorithm:kJWTSignatureAlgorithmHS256];
}

+ (NSString*)encodeClaims:(JWTClaims*)claims withSecret:(NSString*)secret algorithm:(JWTSignatureAlgorithm)algorithm
{
    NSDictionary* payload = [JWTClaims dictionaryWithClaims:claims];
    return [self encodePayload:payload withSecret:secret algorithm:algorithm];
}

+ (NSString*)encodePayload:(NSDictionary*)payload withSecret:(NSString*)secret
{
    return [self encodePayload:payload withSecret:secret algorithm:kJWTSignatureAlgorithmHS256];
}

+ (NSString*)encodePayload:(NSDictionary*)payload withSecret:(NSString*)secret algorithm:(JWTSignatureAlgorithm)algorithm
{
    id<JWTSHAAlgorithm> algorithmObject = (algorithm == kJWTSignatureAlgorithmHS256) ?
                                          [JWTSHAAlgorithmHS256 new] : [JWTSHAAlgorithmHS512 new];
    
    NSDictionary* header = @{@"alg": algorithmObject.name,
                             @"type": @"JWT"};
    
    NSString* headerSegment = [self encodeSegment:header];
    NSString* payloadSegment = [self encodeSegment:payload];
    
    NSString *signingInput = [@[headerSegment, payloadSegment] componentsJoinedByString:@"."];
    NSString *signedOutput = [[algorithmObject encodePayload:signingInput withSecret:secret] urlSafeBase64String];
    return [@[headerSegment, payloadSegment, signedOutput] componentsJoinedByString:@"."];
}

+ (NSString*)encodeSegment:(id)segment
{
    NSError* error = nil;
    NSString* encodedSegment = [[NSJSONSerialization dataWithJSONObject:segment options:0 error:&error] urlSafeBase64String];
    NSAssert(!error, @"Could not encode segment: %@", [error localizedDescription]);
    return encodedSegment;
}

@end
