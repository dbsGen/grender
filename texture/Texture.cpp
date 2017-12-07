//
// Created by gen on 16/5/30.
//

#include "Texture.h"
#include <graphics/Factory.h>

using namespace hirender;
using namespace higraphics;

Texture::Texture() : imp(NULL) {
    texture_info.width = 0;
    texture_info.height = 0;
    channel = DEFAULT;
    frameUpdate = false;
}

Texture::~Texture() {
    if (imp) delete imp;
}

void Texture::setChannel(hirender::Texture::Channel channel) {
    if (this->channel != channel) {
        this->channel = channel;
        if (imp) imp->resetChannel();
        if (!loading)
            clean();
//        update();
    }
}

void Texture::readedBuffer(const void* buffer) {
    getIMP()->draw(texture_info.width, texture_info.height, buffer);
    loaded = true;
}

void Texture::update() {
    getIMP()->update();
}
void Texture::clean() {
    getIMP()->clean();
}
void Texture::load() {
    if (loading) {
        readTexture();
    }else {
        loading = true;
        getIMP()->load();
        loading = false;
    }
}
void Texture::render(int offx, int offy, int width, int height, const void *buffer) {
    getIMP()->render(offx, offy, width, height, buffer);
}

TextureIMP *Texture::getIMP() {
    if (!imp) {
        imp = (TextureIMP*)Factory::getInstance()->create(TextureIMP::getClass());
        imp->_setTarget(this);
    }
    return imp;
}

void TextureIMP::textureLoad() {
    getTarget()->load();
}
