//
//  GL2MeshIMP.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_GL2_MESH_IMP_H
#define HIGRAPHICS_GL2_MESH_IMP_H

#include "include_opengl2.h"

#include <mesh/Mesh.h>

#include <graphics/graphics_define.h>

namespace gg {
    
    CLASS_BEGIN_N(GL2AttrVector, gr::AttrVector)
    
public:
    
    CLASS_END
    
    CLASS_BEGIN_N(GL2MeshIMP, gr::MeshIMP)
    
private:
    
protected:
    virtual gr::AttrVector *newAttribute();
    
public:
    ~GL2MeshIMP();
    
    CLASS_END
}

#endif //HIGRAPHICS_GL2_MESH_IMP_H
