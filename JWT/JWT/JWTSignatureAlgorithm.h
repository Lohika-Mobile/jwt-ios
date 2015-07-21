//
//  JWTSignatureAlgorithm.h
//
//  Created by Alexander Trishyn on 7/6/15.
//  Copyright (c) 2015 Lohika. The MIT License (MIT).
//

#import <Foundation/Foundation.h>

@protocol JWTSHAAlgorithm<NSObject>
@required
@property (nonatomic, readonly, copy) NSString* name;

- (NSData*)encodePayload:(NSString*)data withSecret:(NSString*)secret;

@end

@interface JWTSHAAlgorithmHS256 : NSObject<JWTSHAAlgorithm>
@end

@interface JWTSHAAlgorithmHS512 : NSObject<JWTSHAAlgorithm>
@end
