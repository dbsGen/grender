//
//  GL2TextureIMP.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "GL2TextureIMP.h"
#include <Renderer.h>

#define TEXTURE_ID 0xffff

using namespace higraphics;

GL2TextureIMP::GL2TextureIMP() : TextureIMP() {
    ID = TEXTURE_ID;
    will_reload = false;
    gl_type = GL_UNSIGNED_BYTE;
    gl_internal_formal = gl_format = GL_RGBA;
    pixel_size = 4;
    gl_target = GL_TEXTURE_2D;
}

GL2TextureIMP::~GL2TextureIMP() {
    clean();
}

GLuint GL2TextureIMP::getID() {
    if (ID == TEXTURE_ID) {
        glGenTextures(1, &ID);

        textureLoad();
        will_reload = false;
    }else if (will_reload) {
        textureLoad();
        will_reload = false;
    }
    return ID;
}

void GL2TextureIMP::resetChannel() {
    switch (getTarget()->getChannel()) {
        case Texture::DEFAULT:
        case Texture::RGBA: {
            gl_format = gl_internal_formal = GL_RGBA;
            pixel_size = 4;
        }
            break;
#ifndef __ANDROID_API__
        case Texture::BGRA: {
            gl_format = GL_BGRA;
            gl_internal_formal = GL_RGBA;
            pixel_size = 4;
        }
            break;
#endif
        case Texture::RGB: {
            gl_format = GL_RGB;
            gl_internal_formal = GL_RGB;
            pixel_size = 3;
        }
            break;
        case Texture::ALPHA: {
            gl_format = gl_internal_formal = GL_ALPHA;
            pixel_size = 1;
        }
            break;
        case Texture::LUMINANCE_ALPHA: {
            gl_format = gl_internal_formal = GL_LUMINANCE_ALPHA;
            pixel_size = 2;
        }
            break;
        case Texture::LUMINANCE: {
            gl_format = gl_internal_formal = GL_LUMINANCE;
            pixel_size = 1;
        }
            break;
            
        default:
            gl_format = gl_internal_formal = GL_RGBA;
            pixel_size = 4;
            break;
    }
}

void GL2TextureIMP::_setTarget(Texture *tar) {
    TextureIMP::_setTarget(tar);
    resetChannel();
}

void GL2TextureIMP::clean() {
    if (ID) {
        glDeleteTextures(1, &ID);
        ID = TEXTURE_ID;
    }
}

void GL2TextureIMP::load() {
    if (ID == TEXTURE_ID) {
        getID();
    }else {
        textureLoad();
        will_reload = false;
    }
}

void GL2TextureIMP::display() {
    getID();
}

void GL2TextureIMP::draw(int width, int height, const void *buffer) {
    if (ID != TEXTURE_ID) {
        glBindTexture(gl_target, ID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(gl_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(gl_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        if (buffer) {
            glTexImage2D(gl_target, 0, gl_internal_formal, (GLint)width, (GLint)height, 0, gl_format, gl_type, buffer);
        }else if (getAutoInit()) {
            int size = width * height * pixel_size;
            void *mem = malloc(sizeof(unsigned char) * size);
            memset(mem, 0, size);
            glTexImage2D(gl_target, 0, gl_internal_formal, (GLint)width, (GLint)height, 0, gl_format, gl_type, mem);
            free(mem);
        }
        onDraw();
    }
}

namespace higraphics {
    struct render_struct {
        unsigned int offx, offy, width, height;
        void *buffer;
    };
}

void GL2TextureIMP::_render(void *data, bool release) {
    render_struct *rs = (render_struct*)data;
    if (ID != TEXTURE_ID) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(gl_target, ID);
        if (rs->buffer) {
            glTexSubImage2D(gl_target, 0, (GLint)rs->offx, (GLint)rs->offy, (GLint)rs->width, (GLint)rs->height, gl_format, gl_type, rs->buffer);
        }
    }
    if (release) {
        if (rs->buffer)
            free(rs->buffer);
        delete rs;
    }
}

void GL2TextureIMP::render(unsigned int offx,
                           unsigned int offy,
                           unsigned int width,
                           unsigned int height,
                           const void *buffer) {
    if (Renderer::currentThread() == Renderer::MainThread) {
        render_struct rs;
        rs.offx = offx;
        rs.offy = offy;
        rs.width = width;
        rs.height = height;
        rs.buffer = (void*)buffer;
        _render(&rs, false);
    }else {
        render_struct *rs = new render_struct;
        rs->offx = offx;
        rs->offy = offy;
        rs->width = width;
        rs->height = height;
        size_t s = width * height * pixel_size;
        rs->buffer = malloc(s);
        memcpy(rs->buffer, buffer, s);
        Renderer::sharedInstance()->doOnMainThread(rs, [](void *sender, void *sd, void *data){
            GL2TextureIMP *rimp = (GL2TextureIMP*)data;
            rimp->_render(sd, true);
        }, this);
    }
}

bool GL2TextureIMP::isLoaded() {
    return ID != TEXTURE_ID;
}
