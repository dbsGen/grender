//
//  ColorTexture.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/4.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_COLOR_TEXTURE_H
#define HIRENDER_COLOR_TEXTURE_H

#include "Texture.h"
#include <core/math/Type.h>
#include <render_define.h>

using namespace hicore;

namespace hirender {
    CLASS_BEGIN_N(ColorTexture, Texture)

    private:
        struct __color {
            u_int8_t b[4];

            _FORCE_INLINE_ __color() {
                b[0] = b[1] = b[2] = b[3] = 0xff;
            }
            _FORCE_INLINE_ __color(const Vector4f &c) {
                b[0] = c.red() * 0xff;
                b[1] = c.green() * 0xff;
                b[2] = c.blue() * 0xff;
                b[3] = c.alpha() * 0xff;
            }
        } color;

    protected:
        _FORCE_INLINE_ virtual void readTexture() {
            readedBuffer(color.b);
        }

    public:
        _FORCE_INLINE_ ColorTexture() {
            texture_info.width = 1;
            texture_info.height = 1;
        }
        _FORCE_INLINE_ ColorTexture(const HColor &color) : color(color) {
            texture_info.width = 1;
            texture_info.height = 1;
        }
        _FORCE_INLINE_ void setColor(const HColor &color) {
            this->color = __color(color);
            update();
        }
        _FORCE_INLINE_ Vector4f getColor() {
            return Vector4f(color.b[0]/(float)0xff,
                            color.b[1]/(float)0xff,
                            color.b[2]/(float)0xff,
                            color.b[3]/(float)0xff);
        }
    
    CLASS_END
}

#endif //HIRENDER_COLOR_TEXTURE_H
