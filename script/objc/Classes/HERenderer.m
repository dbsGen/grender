//
//  HERenderer.m
//  hirender_iOS
//
//  Created by Gen on 16/9/21.
//  Copyright © 2016年 gen. All rights reserved.
//

#import "HERenderer.h"

@implementation HERenderer

+ (NSString *)nativeClassName {
    return @"hirender::Renderer";
}

- (void)_requireRender {
    if ([self.delegate respondsToSelector:@selector(requireRender)]) {
        [self.delegate requireRender];
    }
}

@end
