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

using namespace std;
using namespace hicore;

namespace hirender {
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

        vector<Ref<FontTexture> > textures;
        ref_map materials;
        void clear();

        int size;
        Vector2i off;
        int padding;
        int height;

        map<unsigned int, Ref<Character> > characters;

        static const Ref<Shader> &textShader();

        Ref<FontTexture> createTexture();
        void createCharacter(unsigned int ch);

        static map<StringName, Ref<Font> > register_fonts;
    protected:
        virtual void calculateTextureSize(unsigned int &width,
                                          unsigned int &height) = 0;
        virtual void drawCharacter(unsigned int ch, const Vector2i &off, const Ref<Texture> &tex, int &width, int &height, Vector2i &corner) = 0;
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
    
        Ref<Material> getMaterial(const Ref<Texture> &tex);
        Ref<Material> createMaterial(const Ref<Texture> &tex);
        const Ref<Character> &character(unsigned int ch);

        Font(int size);
        ~Font();

        _FORCE_INLINE_ virtual void initialize() {}

        static bool hasFont(const StringName &name);
        static void registerFont(const StringName &name, const Ref<Font> &font);
        static void unregisterFont(const StringName &name);
        static Ref<Font> getFont(const StringName &name);
        static Ref<Font> defaultFont();
    
    CLASS_END
}

#endif /* HIRENDER_FONT_H */
