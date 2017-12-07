//
//  HECallback.m
//  hirender_iOS
//
//  Created by Gen on 16/9/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#import "HECallback.h"

@implementation HECallback

+ (NSString *)nativeClassName {
    return @"hicore::Callback";
}

- (id)_invoke:(NSArray *)params {
    if (self.block)
        return self.block(params);
    if (self.delegate)
        return [self.delegate action:self];
    return NULL;
}

@end
