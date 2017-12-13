
//
//  GL2MeshIMP.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "GL2MeshIMP.h"
#include "GL2MaterialIMP.h"

#define SIZE(SIZE, COUNT, TYPE) size_t SIZE = 0; int COUNT = 0; GLenum t_type; \
switch (TYPE) {\
case Shader::Vecter2:\
SIZE = sizeof(float);\
COUNT = 2; \
t_type = GL_FLOAT; \
break;\
case Shader::Vecter3:\
SIZE = sizeof(float);\
COUNT = 3; \
t_type = GL_FLOAT; \
break;\
case Shader::Vecter4:\
SIZE = sizeof(float);\
COUNT = 4; \
t_type = GL_FLOAT; \
break;\
case Shader::Int:\
case Shader::Long:\
SIZE = sizeof(int);\
COUNT = 1; \
t_type = GL_INT;\
break;\
case Shader::Float:\
case Shader::Double:\
SIZE = sizeof(float);\
COUNT = 1; \
t_type = GL_FLOAT;\
break;\
\
default:\
break;\
}

using namespace higraphics;
using namespace gr;

AttrVector *GL2MeshIMP::newAttribute() {
    return new GL2AttrVector;
}

GL2MeshIMP::~GL2MeshIMP() {
}
