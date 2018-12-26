//
//  ImageTexture.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/2.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "ImageTexture.h"
#include <Renderer.h>
#include <math/Math.hpp>

using namespace gr;
using namespace gc;
using namespace std;

ImageTexture::~ImageTexture() {
    cancelLoad();
}

uint ImageTexture::imageSize(uint input) {
    ResizingType t = getResizingType();
    if (t == ImageTexture::None) {
        return input;
    }
    uint16_t np2 = nextPowerOf2(input);
    if (t == NextPower2) return np2;
    uint16_t lp2 = np2 >> 1;
    if (t == LastPower2) return lp2 ? lp2 : np2;
    return (input > (np2 - (np2 >> 2))) ? np2 : lp2;
}

void ImageTexture::initialize(const string &path) {
    this->path = path;
}

void ImageTexture::initialize(const Ref<Data> &data) {
    this->data = data;
}

void ImageTexture::initialize(const Ref<Data> &raw, int width, int height) {
    this->data = raw;
    raw_image = true;
    texture_info.width = width;
    texture_info.height = height;
}

void ImageTexture::readTexture() {
    if (raw_image && this->data) {
        readedBuffer(this->data->getBuffer());
    }else {
        if (readed_buffer) {
            readedBuffer(readed_buffer);
            free(readed_buffer);
            readed_buffer = NULL;
        }else {
            void *buffer = loadBuffer();
            loaded = true;
            if (buffer) {
                readedBuffer(buffer);
                free(buffer);
            }
        }
    }
}

void ImageTexture::asynThread(const RCallback &on_complete) {
    readed_buffer = loadBuffer();
    loaded = true;
    
    if (!is_cancel) {
        Renderer::sharedInstance()->doOnMainThread(on_complete);
    }else {
        free(readed_buffer);
        readed_buffer = NULL;
    }
    load_thread->detach();
    delete load_thread;
    load_thread = NULL;
}

void ImageTexture::asynLoad(const RCallback &complete) {
    if (!loaded) {
        if (!load_thread) {
            is_cancel = false;
            load_thread = new thread(&ImageTexture::asynThread, this, complete);
        }
    }else {
        complete->invoke(RArray());
    }
}

void ImageTexture::cancelLoad() {
    if (load_thread) {
        is_cancel = true;
        load_thread->join();
    }
}

namespace gr {
    map<string, Ref<ImageTexture> > image_cache;
    list<string> cache_index;
}

#define MAX_CACHE 100

const Ref<ImageTexture> &ImageTexture::cachedTexture(const string &path) {
    while (image_cache.size() > MAX_CACHE) {
        auto it = cache_index.begin();
        image_cache.erase(*it);
        cache_index.erase(it);
    }
    auto it = image_cache.find(path);
    if (it == image_cache.end()) {
        Ref<ImageTexture> tex(new ImageTexture(path));
        image_cache[path] = tex;
        cache_index.push_back(path);
        return image_cache[path];
    }else {
        cache_index.remove(path);
        cache_index.push_back(path);
        return it->second;
    }
}
