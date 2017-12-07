//
//  RenderTexture.hpp
//  hirender_iOS
//
//  Created by mac on 2017/1/18.
//  Copyright © 2017年 gen. All rights reserved.
//

#ifndef RenderTexture_hpp
#define RenderTexture_hpp


#include "Texture.h"
#include <core/math/Type.h>

#include <render_define.h>

namespace hirender {
    CLASS_BEGIN_N(RenderTexture, Texture)
    
    bool resized;
protected:
    virtual void readTexture();
    
public:
    void resetResized();
    _FORCE_INLINE_ bool isResized() { return resized; }
    
    _FORCE_INLINE_ RenderTexture() : resized(true) {
        getIMP()->setAutoInit(true);
    }
    INITIALIZE(RenderTexture, PARAMS(float width, float height),
               texture_info.width = width;
               texture_info.height = height;
               );
    ~RenderTexture();
    
    void setSize(const hicore::HSize &size);
    
    CLASS_END
}

#endif /* RenderTexture_hpp */
