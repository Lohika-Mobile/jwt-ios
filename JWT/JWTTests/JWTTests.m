//
//  JWTTests.m
//  JWTTests
//
//  Created by Alexander Trishyn on 7/21/15.
//  Copyright (c) 2015 Lohika. The MIT License (MIT).
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import <JWT.h>

@interface JWTTests : XCTestCase

@end

@implementation JWTTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testJWTPayloadEncode {
    NSString* jwt =[JWT encodePayload:@{@"payload" : @"data"} withSecret:@"secret"];
    XCTAssertEqualObjects(jwt,
                          @"eyJhbGciOiJIUzI1NiIsInR5cGUiOiJKV1QifQ.eyJwYXlsb2FkIjoiZGF0YSJ9.fJGS95mRwT2JBKmq63-NLhJdSEqTca_-zBeEgsm8eyU", @"Pass");
}

@end
