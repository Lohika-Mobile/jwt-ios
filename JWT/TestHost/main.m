//
//  main.m
//  TestHost
//
//  Created by Alexander Trishyn on 7/21/15.
//  Copyright (c) 2015 Lohika. The MIT License (MIT).
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
#if TARGET_IPHONE_SIMULATOR
#else
    const char *prefix = "GCOV_PREFIX";
    const char *prefixValue = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"]
                               cStringUsingEncoding:NSASCIIStringEncoding]; // This gets the filepath to the app's Documents directory
    const char *prefixStrip = "GCOV_PREFIX_STRIP";
    const char *prefixStripValue = "7";
    setenv(prefix, prefixValue, 1); // This sets an environment variable which tells gcov where to put the .gcda files.
    setenv(prefixStrip, prefixStripValue, 1); // This tells gcov to strip the default prefix, and use the filepath that we just declared.
#endif
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
