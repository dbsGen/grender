//
//  include_opengl2.h
//  hirender_iOS
//
//  Created by gen on 16/6/5.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_GL2_INCLUDE_OPENGL2_H
#define HIGRAPHICS_GL2_INCLUDE_OPENGL2_H

#if defined(__APPLE__)

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#elif defined(__ANDROID__)

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif

#endif //HIGRAPHICS_GL2_INCLUDE_OPENGL2_H
