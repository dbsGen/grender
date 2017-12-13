//
//  ImageTexture_apple.cpp
//  hirender_iOS
//
//  Created by gen on 29/12/2016.
//  Copyright © 2016 gen. All rights reserved.
//

#ifdef __OBJC__

#import <CoreImage/CoreImage.h>
#import <UIKit/UIKit.h>
#include "ImageTexture.h"

#define ui_image ((CGImageRef)img)

using namespace gr;


void *ImageTexture::loadBuffer() {
    @autoreleasepool {
        UIImage *image = nil;
        
        if (data && !data->empty()) {
            image = [UIImage imageWithData:[NSData dataWithBytes:data->getBuffer()
                                                          length:data->getSize()]];
        }else if (!path.empty())
            image = [UIImage imageWithContentsOfFile:[NSString stringWithUTF8String:path.c_str()]];
        else {
            return NULL;
        }
        if (image) {
            texture_info.width = imageSize(image.size.width);
            texture_info.height = imageSize(image.size.height);
            CIImage *ci_image = [CIImage imageWithCGImage:image.CGImage];
            if (texture_info.width != image.size.width || texture_info.height != image.size.height) {
                ci_image = [ci_image imageByApplyingTransform:CGAffineTransformMakeScale(texture_info.width/image.size.width,
                                                                                         texture_info.height/image.size.height)];
            }
            
            setChannel(BGRA);
            CGRect bounds = CGRectMake(0, 0, texture_info.width, texture_info.height);
            void *buffer = malloc(texture_info.width * texture_info.height * 4);
            [[CIContext context] render:ci_image
                               toBitmap:buffer
                               rowBytes:texture_info.width * 4
                                 bounds:bounds
                                 format:kCIFormatBGRA8
                             colorSpace:nil];
            return buffer;
        }
    }
    return NULL;
}

void ImageTexture::readSize() {
    @autoreleasepool {
        UIImage *image = nil;
        if (data && !data->empty()) {
            image = [UIImage imageWithData:[NSData dataWithBytes:data->getBuffer()
                                                          length:data->getSize()]];
        }else if (path.size())
            image = [UIImage imageWithContentsOfFile:[NSString stringWithUTF8String:path.c_str()]];
        else {
            return;
        }
        
        if (image) {
            texture_info.width = imageSize(image.size.width);
            texture_info.height = imageSize(image.size.height);
        }
    }
}

#endif
