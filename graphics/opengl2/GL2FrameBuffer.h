//
//  GL2FrameBuffer.hpp
//  hirender_iOS
//
//  Created by mac on 2017/1/15.
//  Copyright © 2017年 gen. All rights reserved.
//

#ifndef GL2FrameBuffer_hpp
#define GL2FrameBuffer_hpp

#include <core/Object.h>
#include "include_opengl2.h"

namespace gg {
    CLASS_BEGIN_0_N(GL2FrameBuffer)
    
    GLuint  ID;
    GLuint  depth_buffer;
    bool    binded;
    
    Object *target;
    
public:
    GL2FrameBuffer();
    ~GL2FrameBuffer();
    
    _FORCE_INLINE_ void bind(Object *target) {
        this->target = target;
        binded = false;
    }
    
    void clear();
    
    void prepare();
    
    CLASS_END
}

#endif /* GL2FrameBuffer_hpp */
