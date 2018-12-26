//
//  Font.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_FONT_H
#define HIRENDER_FONT_H

#include <core/core.h>
#include <texture/Texture.h>
#include <core/Data.h>
#include "Character.h"
#include "../render_define.h"

namespace gr {
    CLASS_BEGIN_0_NV(Font)

    protected:
        CLASS_BEGIN_N(FontTexture, Texture)

        protected:
            virtual void readTexture();
        public:
            _FORCE_INLINE_ FontTexture():Texture() {
                setChannel(Texture::ALPHA);
                getIMP()->setAutoInit(true);
            }
            _FORCE_INLINE_ FontTexture(int width, int height) : FontTexture(){
                texture_info.width = width;
                texture_info.height = height;
                getIMP()->display();
            }

        CLASS_END

    private:

        std::vector<gc::Ref<FontTexture> > textures;
        ref_map materials;
        void clear();

        int size;
        Vector2i off;
        int padding;
        int height;

        std::map<unsigned int, gc::Ref<Character> > characters;

        static const gc::Ref<Shader> &textShader();

        gc::Ref<FontTexture> createTexture();
        void createCharacter(unsigned int ch);

        static std::map<gc::StringName, gc::Ref<Font> > register_fonts;
    protected:
        virtual void calculateTextureSize(unsigned int &width,
                                          unsigned int &height) = 0;
        virtual void drawCharacter(unsigned int ch, const Vector2i &off, const gc::Ref<Texture> &tex, int &width, int &height, Vector2i &corner) = 0;
        _FORCE_INLINE_ void setHeight(int height) {this->height = height;}

    public:
        _FORCE_INLINE_ int getHeight() {
            return height;
        }
    
        virtual Vector2f offset(unsigned int chLeft, unsigned int chRight);
        _FORCE_INLINE_ int getSize() const {return size;}

        _FORCE_INLINE_ int getPadding() {
            return padding;
        }

        _FORCE_INLINE_ void setPadding(int padding) {
            this->padding = padding;
        }
    
        gc::Ref<Material> getMaterial(const gc::Ref<Texture> &tex);
        gc::Ref<Material> createMaterial(const gc::Ref<Texture> &tex);
        const gc::Ref<Character> &character(unsigned int ch);

        Font(int size);
        ~Font();

        _FORCE_INLINE_ virtual void initialize() {}

        static bool hasFont(const gc::StringName &name);
        static void registerFont(const gc::StringName &name, const gc::Ref<Font> &font);
        static void unregisterFont(const gc::StringName &name);
        static gc::Ref<Font> getFont(const gc::StringName &name);
        static gc::Ref<Font> defaultFont();
    
    CLASS_END
}

#endif /* HIRENDER_FONT_H */
