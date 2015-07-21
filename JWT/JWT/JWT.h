//
//  JWT.h
//  JWT
//
//  Created by Alexander Trishyn on 7/21/15.
//  Copyright (c) 2015 Lohika. The MIT License (MIT).
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSUInteger, JWTSignatureAlgorithm)
{
    kJWTSignatureAlgorithmHS256 = 0,
    kJWTSignatureAlgorithmHS512 = 1
};

@interface JWTClaims : NSObject
@property (nonatomic, copy) NSString* issuer;
@property (nonatomic, copy) NSString* subject;
@property (nonatomic, copy) NSString* audience;
@property (nonatomic, copy) NSDate* expirationDate;
@property (nonatomic, copy) NSDate* notBeforeDate;
@property (nonatomic, copy) NSDate* issuedAt;
@property (nonatomic, copy) NSString* identifier;
@property (nonatomic, copy) NSString* type;
@end

@interface JWT : NSObject

+ (NSString*)version;

+ (NSString*)encodeClaims:(JWTClaims*)claims withSecret:(NSString*)secret;
+ (NSString*)encodeClaims:(JWTClaims*)claims withSecret:(NSString*)secret algorithm:(JWTSignatureAlgorithm)algorithm;

+ (NSString*)encodePayload:(NSDictionary*)payload withSecret:(NSString*)secret;
+ (NSString*)encodePayload:(NSDictionary*)payload withSecret:(NSString*)secret algorithm:(JWTSignatureAlgorithm)algorithm;

@end
