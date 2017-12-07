//
//  ImageTexture.h
//  hirender_iOS
//
//  Created by gen on 16/6/2.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_IMAGE_TEXTURE_H
#define HIRENDER_IMAGE_TEXTURE_H

#include "Texture.h"
#include <core/Data.h>
#include <thread>
#include <atomic>
#include <core/Callback.h>
#include "../render_define.h"

using namespace std;
using namespace hicore;

namespace hirender {
    
    CLASS_BEGIN_N(ImageTexture, Texture)
    public:
        enum _ResizingType {
            NextPower2,
            LastPower2,
            NearPower2,
            None
        };
        typedef int ResizingType;
    private:
        bool loaded;
        Ref<Data> data;
        string path;
        ResizingType resizing_type;
        bool raw_image;
        uint imageSize(uint input);
    
        atomic<bool> is_cancel;
        thread *load_thread;
        void *readed_buffer;
    
        void asynThread(Ref<Callback> on_complete);

    protected:
        virtual void readSize();
        virtual void readTexture();
    
        void *loadBuffer();

    public:
        _FORCE_INLINE_ ImageTexture() : Texture(), resizing_type(NearPower2), raw_image(false), readed_buffer(NULL), loaded(false) {
            load_thread = NULL;
        }
        ~ImageTexture();

        METHOD _FORCE_INLINE_ void setPath(const string &path) {
            this->path = path;
            data = nullptr;
            update();
        }
        METHOD _FORCE_INLINE_ const string &getPath() {
            return path;
        }
        METHOD _FORCE_INLINE_ void setData(const Ref<Data> &data) {
            this->data = data;
            update();
        }
        METHOD _FORCE_INLINE_ const Ref<Data> &getData() {
            return data;
        }
        _FORCE_INLINE_ void setResizingType(ResizingType type) {
            resizing_type = type;
        }
        _FORCE_INLINE_ ResizingType getResizingType() {
            return resizing_type;
        }

        _FORCE_INLINE_ ImageTexture(const string &path) : ImageTexture() {
            initialize(path);
        }
        _FORCE_INLINE_ ImageTexture(const Ref<Data> &data) : ImageTexture() {
            initialize(data);
        }
        _FORCE_INLINE_ ImageTexture(const Ref<Data> &raw, int width, int height) : ImageTexture() {
            initialize(raw, width, height);
        }
        void initialize(const string &path);
        void initialize(const Ref<Data> &data);
        void initialize(const Ref<Data> &raw, int width, int height);
    
        static const Ref<ImageTexture> &cachedTexture(const string &path);
        void asynLoad(const Ref<Callback> &complete);
        void cancelLoad();

    protected:
        ON_LOADED_BEGIN(cls, Texture)
            ADD_METHOD(cls, ImageTexture, setPath);
            ADD_METHOD(cls, ImageTexture, getPath);
            ADD_METHOD(cls, ImageTexture, setData);
            ADD_METHOD(cls, ImageTexture, getData);
        ON_LOADED_END
    CLASS_END
}

#endif //HIRENDER_IMAGE_TEXTURE_H
