//
//  AppleFont.cpp
//  hirender_iOS
//
//  Created by mac on 2016/12/30.
//  Copyright © 2016年 gen. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>
#include "AppleFont.h"

using namespace hirender;

AppleFont::AppleFont() : AppleFont([UIFont systemFontOfSize:[UIFont systemFontSize]]) {
    
}

AppleFont::AppleFont(UIFont *font) : Font((int)ceil(font.pointSize)), context(NULL), font_dic(NULL), font_path(NULL) {
    this->font = [font retain];
    setHeight((int)ceil(font.lineHeight));
}

AppleFont::AppleFont(const char *name, int size) : AppleFont([UIFont fontWithName:[NSString stringWithUTF8String:name]
                                                                             size:size]) {
}

AppleFont::~AppleFont() {
    [font release];
    if (context) {
        CGContextRelease(context);
    }
    [font_dic release];
    if (font_path) {
        CGPathRelease(font_path);
    }
}

void AppleFont::calculateTextureSize(unsigned int &width, unsigned int &height) {
    width = height = 2048;
}

void AppleFont::drawCharacter(unsigned int ch, const hicore::Vector2i &off, const Ref<hirender::Texture> &tex, int &width, int &height, hicore::Vector2i &corner) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    if (!context) {
        float s = getHeight();
        context_rect = CGRectMake(0, 0, s, s);
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
        context = CGBitmapContextCreate(NULL, s, s, 8, s, colorSpace, kCGImageAlphaOnly);
        
        CGColorSpaceRelease(colorSpace);
        font_dic = [[NSDictionary alloc] initWithObjectsAndKeys:font, NSFontAttributeName, nil];
        
        
        CGMutablePathRef path = CGPathCreateMutable();
        CGPathAddRect(path, NULL, CGRectMake(0, 0, context_rect.size.width, context_rect.size.height));
        font_path = path;
    }
    
    NSString *tx = [NSString stringWithCharacters:(const unichar *)&ch
                                           length:1];
    CGRect bounds = [tx boundingRectWithSize:context_rect.size
                                     options:NSStringDrawingUsesLineFragmentOrigin
                                  attributes:font_dic
                                     context:nil];
    CGContextClearRect(context, context_rect);
    
    CFAttributedStringRef attr_string = CFAttributedStringCreate(nil, (CFStringRef)tx, (CFDictionaryRef)font_dic);
    CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(attr_string);
    CTFrameRef draw_frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0), font_path, NULL);
    CTFrameDraw(draw_frame, context);
    CFRelease(draw_frame);
    CFRelease(framesetter);
    CFRelease(attr_string);
    
    width = (int)floor(bounds.size.width);
    height = (int)ceil(font.lineHeight + font.descender);
    corner.x(0);
    corner.y(0);
    char *bfs = (char *)CGBitmapContextGetData(context);
    tex->render(off.x(), off.y(), getHeight(), getHeight(), bfs);
    
    [pool release];
}
