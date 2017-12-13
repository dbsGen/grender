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

#include "../render_define.h"

using namespace std;
using namespace gcore;

namespace cv {
    class Mat;
}

namespace gr {
    
    CLASS_BEGIN_N(ImageTexture, Texture)
    private:
        bool loaded;
        cv::Mat *mat;
        Ref<Data> data;
        string path;
    
        void readPicture();

    protected:
        virtual void readSize();
        virtual void readTexture();

    public:
        _FORCE_INLINE_ ImageTexture() : Texture() {mat = NULL;}
        ~ImageTexture();

        METHOD _FORCE_INLINE_ void setPath(string path) {
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

        _FORCE_INLINE_ ImageTexture(const string &path) : ImageTexture() {
            initialize(path);
        }
        _FORCE_INLINE_ ImageTexture(const Ref<Data> &data) : ImageTexture() {
            initialize(data);
        }
        void initialize(const string &path);
        void initialize(const Ref<Data> &data);

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
