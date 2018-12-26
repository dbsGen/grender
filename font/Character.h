//
//  Character.h
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_CHARACTER_H
#define HIRENDER_CHARACTER_H

#include <core/core.h>
#include <texture/Texture.h>
#include <material/Material.h>
#include "../math/Type.h"
#include "../render_define.h"

namespace gr {

    class Font;

    CLASS_BEGIN_0_N(Character)

        unsigned int width, height;
        Vector2i corner;

        Vector2f uv[2];

        unsigned long chr;

        gc::Ref<Texture> texture;
        Font *font;
        bool isRender;

        friend class Font;

    public:
        _FORCE_INLINE_ unsigned long getChar() {return chr;}
        _FORCE_INLINE_ unsigned int getWidth() { return width; }
        _FORCE_INLINE_ unsigned int getHeight() { return height; }
        _FORCE_INLINE_ const Vector2f *getUV() { return uv; }
        _FORCE_INLINE_ const Vector2i &getCorner() { return corner; }
        _FORCE_INLINE_ const gc::Ref<Texture> &getTexture() { return texture; }
        _FORCE_INLINE_ Font *getFont() {return font;}

        _FORCE_INLINE_ Character():isRender(false){}
        _FORCE_INLINE_ Character(unsigned long ch,
                                 unsigned int offX,
                                 unsigned int offY,
                                 unsigned int width,
                                 unsigned int height,
                                 unsigned int texWidth,
                                 unsigned int texHeight,
                                 unsigned int left,
                                 unsigned int top,
                                 const gc::Ref<Texture> &texture,
                                 Font *font):Character() {
            this->chr = ch;
            corner = Vector2i(left,top);
            this->width = width;
            this->height = height;
            this->texture = texture;
            uv[0].x(offX / (float)texWidth);
            uv[0].y(offY / (float)texHeight);
            uv[1].x((offX+width) / (float)texWidth);
            uv[1].y((offY+height) / (float)texHeight);
            this->font = font;
        }

        _FORCE_INLINE_ Character(const Character &other) {
            operator=(other);
        }

        _FORCE_INLINE_ Character &operator=(const Character &other) {
            chr = other.chr;
            corner = other.corner;
            width = other.width;
            height = other.height;
            texture = other.texture;
            uv[0] = other.uv[0];
            uv[1] = other.uv[1];
            font = other.font;
            isRender = other.isRender;
            return *this;
        }
    
    CLASS_END
}

#endif /* HIRENDER_CHARACTER_H */
