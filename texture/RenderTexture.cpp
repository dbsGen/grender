//
//  RenderTexture.cpp
//  hirender_iOS
//
//  Created by mac on 2017/1/18.
//  Copyright © 2017年 gen. All rights reserved.
//

#include "RenderTexture.h"

using namespace gr;

void RenderTexture::readTexture() {
    readedBuffer(NULL);
}

RenderTexture::~RenderTexture() {
    LOG(i, "Render delete");
}

void RenderTexture::setSize(const HSize &size) {
    if (texture_info.width != size.x() || texture_info.height != size.y()) {
        texture_info.width = size.x();
        texture_info.height = size.y();
        clean();
        resized = true;
    }
}

void RenderTexture::resetResized() {
    resized = false;
}
