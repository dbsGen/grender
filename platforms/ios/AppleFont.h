//
//  AppleFont.hpp
//  hirender_iOS
//
//  Created by mac on 2016/12/30.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef AppleFont_hpp
#define AppleFont_hpp

#import <UIKit/UIKit.h>
#include <font/Font.h>
#include "../../render_define.h"

namespace gr {
    CLASS_BEGIN_N(AppleFont, Font)
    
    UIFont          *font;
    
    CGContextRef    context;
    CGRect          context_rect;
    NSDictionary    *font_dic;
    CGPathRef       font_path;
    
protected:
    virtual void calculateTextureSize(unsigned int &width,
                                      unsigned int &height);
    virtual void drawCharacter(unsigned int ch,
                               const Vector2i &off,
                               const Ref<Texture> &tex,
                               int &width, int &height,
                               Vector2i &corner);
    
public:
    AppleFont();
    AppleFont(UIFont *font);
    AppleFont(const char *name, int size);
    
    ~AppleFont();
    
    CLASS_END
}

#endif /* AppleFont_hpp */
