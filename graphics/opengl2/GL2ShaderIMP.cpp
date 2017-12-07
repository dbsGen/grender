//
//  GL2ShaderIMP.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "GL2ShaderIMP.h"
#include <set>

using namespace higraphics;
using namespace std;

GLuint GL2ShaderIMP::compile(GLenum type, const char *shader) {
    GLuint ret;
    GLint status;
    const GLchar *source;
    
    source = (const GLchar *)shader;
    if (!source)
        return 0;
    
    ret = glCreateShader(type);
    glShaderSource(ret, 1, &source, NULL);
    glCompileShader(ret);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(ret, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(ret, logLength, &logLength, log);
        LOG(e, "Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(ret, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(ret);
        LOG(e, "Can not compile shader");
        return 0;
    }
    
    return ret;
}


