//
//  HEiOSPlatform.m
//  hirender_iOS
//
//  Created by Gen on 16/9/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#import "HEiOSPlatform.h"

@implementation HEiOSPlatform

+ (NSString *)nativeClassName {
    return @"hirender::Platform";
}

- (NSString*)_persistencePath {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return paths.firstObject;
}

- (void)_startInput:(NSString *)text :(NSString *)placeholder :(HECallback *)callback {
    
}

- (void)_endInput {
    
}

- (void)setCallback:(HECallback *)callback {
    if (callback != _callback) {
        [_callback release];
        _callback = callback;
        [_callback retain];
        
        [self setTestCallback:callback];
    }
}

@end
