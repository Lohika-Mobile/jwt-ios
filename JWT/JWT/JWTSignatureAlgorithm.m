//
//  JWTSignatureAlgorithm.m
//
//  Created by Alexander Trishyn on 7/6/15.
//  Copyright (c) 2015 Lohika. The MIT License (MIT).
//

#import "JWTSignatureAlgorithm.h"

#import <CommonCrypto/CommonCrypto.h>
#import <CommonCrypto/CommonHMAC.h>

@implementation JWTSHAAlgorithmHS512

- (NSString*)name;
{
    return @"HS512";
}

- (NSData*)encodePayload:(NSString*)data withSecret:(NSString*)secret
{
    const char* cStringData = [data cStringUsingEncoding:NSUTF8StringEncoding];
    const char* cSecret = [secret cStringUsingEncoding:NSUTF8StringEncoding];
    
    unsigned char cHMAC[CC_SHA512_DIGEST_LENGTH];
    CCHmac(kCCHmacAlgSHA512, cSecret, strlen(cSecret), cStringData, strlen(cStringData), cHMAC);
    return [[NSData alloc] initWithBytes:cHMAC length:sizeof(cHMAC)];
}

@end

#pragma mark-

@implementation JWTSHAAlgorithmHS256

- (NSString*)name;
{
    return @"HS256";
}

- (NSData*)encodePayload:(NSString*)data withSecret:(NSString*)secret
{
    const char* cStringData = [data cStringUsingEncoding:NSUTF8StringEncoding];
    const char* cSecret = [secret cStringUsingEncoding:NSUTF8StringEncoding];
    
    unsigned char cHMAC[CC_SHA256_DIGEST_LENGTH];
    memset(cHMAC, 0, sizeof(cHMAC));
    CCHmac(kCCHmacAlgSHA256, cSecret, strlen(cSecret), cStringData, strlen(cStringData), cHMAC);
    return [[NSData alloc] initWithBytes:cHMAC length:sizeof(cHMAC)];
}

@end