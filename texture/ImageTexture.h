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

namespace gr {
    
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
        gc::Ref<gc::Data> data;
        std::string path;
        ResizingType resizing_type;
        bool raw_image;
        uint imageSize(uint input);
    
        std::atomic<bool> is_cancel;
        std::thread *load_thread;
        void *readed_buffer;
    
        void asynThread(const gc::RCallback &on_complete);

    protected:
        virtual void readSize();
        virtual void readTexture();
    
        void *loadBuffer();

    public:
        _FORCE_INLINE_ ImageTexture() : Texture(), resizing_type(NearPower2), raw_image(false), readed_buffer(NULL), loaded(false) {
            load_thread = NULL;
        }
        ~ImageTexture();

        METHOD _FORCE_INLINE_ void setPath(const std::string &path) {
            this->path = path;
            data = nullptr;
            update();
        }
        METHOD _FORCE_INLINE_ const std::string &getPath() {
            return path;
        }
        METHOD _FORCE_INLINE_ void setData(const gc::Ref<gc::Data> &data) {
            this->data = data;
            update();
        }
        METHOD _FORCE_INLINE_ const gc::Ref<gc::Data> &getData() {
            return data;
        }
        _FORCE_INLINE_ void setResizingType(ResizingType type) {
            resizing_type = type;
        }
        _FORCE_INLINE_ ResizingType getResizingType() {
            return resizing_type;
        }

        _FORCE_INLINE_ ImageTexture(const std::string &path) : ImageTexture() {
            initialize(path);
        }
        _FORCE_INLINE_ ImageTexture(const gc::Ref<gc::Data> &data) : ImageTexture() {
            initialize(data);
        }
        _FORCE_INLINE_ ImageTexture(const gc::Ref<gc::Data> &raw, int width, int height) : ImageTexture() {
            initialize(raw, width, height);
        }
        void initialize(const std::string &path);
        void initialize(const gc::Ref<gc::Data> &data);
        void initialize(const gc::Ref<gc::Data> &raw, int width, int height);
    
        static const gc::Ref<ImageTexture> &cachedTexture(const std::string &path);
        void asynLoad(const gc::RCallback &complete);
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
