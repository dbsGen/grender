//
//  ProxyTexture.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/4.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_PROXY_TEXTURE_H
#define HIRENDER_PROXY_TEXTURE_H

#include "Texture.h"
#include <render_define.h>

namespace gr {
    CLASS_BEGIN_N(ProxyTexture, Texture)
public:
    typedef const void * (*ProxyTextureReader)(uint &width, uint &height, void *data);
    
private:
    ProxyTextureReader proxyBlock;
    void *data;
    
protected:
    _FORCE_INLINE_ virtual void readSize() {proxyBlock(texture_info.width, texture_info.height, data);}
    _FORCE_INLINE_ virtual void readTexture() {readedBuffer(proxyBlock(texture_info.width, texture_info.height, data));}
    
public:
    
    _FORCE_INLINE_ void setBlock(ProxyTextureReader block, void *data) {proxyBlock = block;this->data = data;}
    
    CLASS_END
}


#endif // HIRENDER_PROXY_TEXTURE_H
