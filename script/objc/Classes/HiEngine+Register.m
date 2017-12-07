//
//  HiEngine+Register.m
//  hirender_iOS
//
//  Created by Gen on 16/9/21.
//  Copyright © 2016年 gen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "../HiObject.h"
#import "HERenderer.h"
#import "HEiOSPlatform.h"
#import "HECallback.h"
#import "HEController.h"
#import "HENavigationController.h"
#import "HEScriptTransport.h"
#import "HEObject.h"
#import "HEView.h"
#import "HEImageView.h"
#import "HEImageTexture.h"

@implementation HiEngine (Register)

+ (void)registerClasses {
    HiEngine *engine = [HiEngine getInstance];
    [engine reg:[HERenderer class]];
    [engine reg:[HEiOSPlatform class]];
    [engine reg:[HECallback class]];
    [engine reg:[HEController class]];
    [engine reg:[HENavigationController class]];
    [engine reg:[HEScriptTransport class]];
    [engine reg:[HEObject class]];
    [engine reg:[HEView class]];
    [engine reg:[HEImageView class]];
    [engine reg:[HEImageTexture class]];
    //    [HiEngine reg:@"hirender::Renderer" protocol:@protocol(HRRenderer)];
}

@end
