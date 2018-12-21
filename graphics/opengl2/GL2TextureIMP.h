//
//  GL2TextureIMP.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_GL2_TEXTURE_IMP_H
#define HIGRAPHICS_GL2_TEXTURE_IMP_H

#include "include_opengl2.h"

#include <texture/Texture.h>
#include <graphics/graphics_define.h>

namespace higraphics {
    CLASS_BEGIN_N(GL2TextureIMP, TextureIMP)

    private:
        GLuint ID;
        bool will_reload;

        GLint gl_internal_formal;
        GLenum gl_target;
        GLenum gl_type;
        GLenum gl_format;
        int pixel_size;
    
    void _render(void *data, bool release);

    protected:
        _FORCE_INLINE_ virtual void update(){
            will_reload = true;
        }
        virtual void resetChannel();
        virtual void clean();
        virtual void load();
        virtual void render(unsigned int offx, unsigned int offy, unsigned int width, unsigned int height, const void *buffer);
        virtual void draw(int width, int height, const void *buffer);

    public:
        virtual void display();
        virtual bool isLoaded();

        virtual void _setTarget(Texture *tar);
        GLuint getID();

        _FORCE_INLINE_ GLint getGLInternalFormat() {
            return gl_internal_formal;
        }
        _FORCE_INLINE_ void setGLInternalFormat(GLint format) {
            gl_internal_formal = format;
        }
        _FORCE_INLINE_ GLenum getGLType() {
            return gl_type;
        }
        _FORCE_INLINE_ void setGLType(GLenum type) {
            this->gl_type = type;
        }
        _FORCE_INLINE_ GLenum getGLTarget() {
            return gl_target;
        }
        _FORCE_INLINE_ void setGLTarget(GLenum target) {
            this->gl_target = target;
        }
        _FORCE_INLINE_ GLenum getGLFormat() {
            return gl_format;
        }
        _FORCE_INLINE_ void setGLFormat(GLenum format) {
            this->gl_format = format;
        }

        GL2TextureIMP();
        ~GL2TextureIMP();

    CLASS_END
}

#endif /* HIGRAPHICS_GL2_TEXTURE_IMP_H */
