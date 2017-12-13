//
//  GL2FrameBuffer.cpp
//  hirender_iOS
//
//  Created by mac on 2017/1/15.
//  Copyright © 2017年 gen. All rights reserved.
//

#include "GL2FrameBuffer.h"
#include "GL2TextureIMP.h"


using namespace higraphics;

GL2FrameBuffer::GL2FrameBuffer() : ID(0), depth_buffer(0), target(NULL), binded(false) {
}

GL2FrameBuffer::~GL2FrameBuffer() {
    clear();
}

void GL2FrameBuffer::clear() {
    if (ID)
    {
        glDeleteFramebuffers(1, &ID);
        ID = 0;
    }
    if (depth_buffer)
    {
        glDeleteRenderbuffers(1, &depth_buffer);
        depth_buffer = 0;
    }
    binded = false;
}

void GL2FrameBuffer::prepare() {
    if (target) {
        if (!ID) {
            glGenFramebuffers(1, &ID);
        }
        if (ID) {
            glBindFramebuffer(GL_FRAMEBUFFER, ID);
            if (!binded) {
                GL2TextureIMP *gl_tex = target->cast_to<GL2TextureIMP>();
                if (gl_tex) {
                    Texture *tex = gl_tex->getTarget();
                    uint16_t width = tex->getWidth();
                    uint16_t height = tex->getHeight();
                    
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_tex->getID(), 0);
                    
                    if (depth_buffer) {
                        glDeleteRenderbuffers(1, &depth_buffer);
                    }
                    glGenRenderbuffers(1, &depth_buffer);
                    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
                    binded = true;
                }
            }
        }
    }
}
