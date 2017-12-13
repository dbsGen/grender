//
//  Font.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "Font.h"

#define RENDER_PADDING 3

using namespace gr;

#define Check(ERR) if (ERR) {LOG(e, "Error in font.");return;}


map<StringName, Ref<Font> > Font::register_fonts;

const Ref<Shader> &Font::textShader() {
    static Ref<Shader> text_shader(new Shader("text.vert", "text.frag"));
    return text_shader;
}

void Font::FontTexture::readTexture() {
    readedBuffer(NULL);
}

Font::Font(int size): off(0,0), padding(RENDER_PADDING), size(size) {
    height = size;
}

void Font::clear() {
    textures.clear();
    characters.clear();
}

const Ref<Character> &Font::character(unsigned int ch) {
    auto it = characters.find(ch);
    if (it == characters.end()) {
        createCharacter(ch);
        return characters[ch];
    }else {
        return it->second;
    }
}

void Font::createCharacter(unsigned int ch) {
    if (textures.empty()) {
        const Ref<FontTexture> &tex = createTexture();
        textures.push_back(tex);
        off.v[0] = off.v[1] = padding;
    }
    Ref<FontTexture> &tex = textures.back();
    if (off.x()> tex->getWidth() - height - padding) {
        off.x(padding);
        off.v[1] += height + padding;
        
        if (off.y() > tex->getHeight() - height - padding) {
            tex = createTexture();
            textures.push_back(tex);
            off.y(padding);
        }
    }

    int width, height;
    Vector2i corner;
    drawCharacter(ch, off, tex, width, height, corner);
    
    Ref<Character> character = new Character(ch, off.x(), off.y(),
                                             width, height,
                                             tex->getWidth(), tex->getHeight(),
                                             corner.x(),
                                             corner.y(),
                                             tex, this);
    characters[ch] = character;
    off.v[0] += width + padding;
}

Ref<Font::FontTexture> Font::createTexture() {
    unsigned int width, height;
    calculateTextureSize(width, height);
    Ref<Font::FontTexture> tex(new Font::FontTexture(width, height));
    
    return tex;
}

Ref<Material> Font::createMaterial(const Ref<Texture> &tex) {
    Ref<Material> mat(new Material(textShader()));
    mat->setBlend(true);
    mat->setTexture(tex, 0);
    mat->setUniform("ALPHA", 1);
    return mat;
}

Ref<Material> Font::getMaterial(const Ref<Texture> &tex) {
    auto it = materials.find(*tex);
    if (it == materials.end()) {
        Ref<Material> mat = createMaterial(tex);
        materials[*tex] = mat;
        return mat;
    }else {
        return it->second;
    }
}

void Font::registerFont(const StringName &name, const Ref<Font> &font) {
//    if (registerFonts.find(name) == registerFonts.end())
    register_fonts[name] = font;
}

void Font::unregisterFont(const StringName &name) {
    register_fonts.erase(name);
}

Ref<Font> Font::getFont(const StringName &name) {
    auto it = register_fonts.find(name);
    if (it != register_fonts.end()) {
        return it->second;
    }
    return Ref<Font>::null();
}

Ref<Font> Font::defaultFont() {
    if (register_fonts.size()) {
        return register_fonts.begin()->second;
    }
    return Ref<Font>::null();
}

Font::~Font() {
}

bool Font::hasFont(const StringName &name) {
    return register_fonts.find(name) != register_fonts.end();
}

Vector2f Font::offset(unsigned int chLeft, unsigned int chRight) {
    return gcore::Vector2f();
}


