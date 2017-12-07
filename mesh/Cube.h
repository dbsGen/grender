//
//  Cube.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/13.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_CUBE_H
#define HIRENDER_CUBE_H

#include "Mesh.h"
#include <render_define.h>

namespace hirender {
    CLASS_BEGIN_N(Cube, Mesh)
private:
    Vector3f size;
    void updateVertexes();
    void createVertexes();
    
public:
    Cube();
    INITIALIZE(Cube, const Vector3f &size,
               this->size = size;
               updateVertexes();
               )
    
    _FORCE_INLINE_ const Vector3f &getSize() {return size;}
    
    _FORCE_INLINE_ void setSize(const Vector3f &size) {this->size = size;updateVertexes();}
    
    CLASS_END
}

#endif /* HIRENDER_CUBE_H */
