//
//  GL2ShaderIMP.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_GL2_SHADER_IMP_H
#define HIGRAPHICS_GL2_SHADER_IMP_H

#include "include_opengl2.h"

#include <material/Shader.h>

#include <graphics/graphics_define.h>

using namespace hirender;

namespace higraphics {
    CLASS_BEGIN_N(GL2ShaderIMP, ShaderIMP)
    
protected:
    
    GLuint compile(GLenum type, const char *shader);
    
public:
    _FORCE_INLINE_ GLuint compileVertex() {return compile(GL_VERTEX_SHADER, getShader(0));}
    _FORCE_INLINE_ GLuint compileFragment() {return compile(GL_FRAGMENT_SHADER, getShader(1));}
    
    CLASS_END
}

#endif //HIGRAPHICS_GL2_SHADER_IMP_H
