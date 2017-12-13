//
//  GL2MaterialIMP.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_GL2_MATERIAL_IMP_H
#define HIGRAPHICS_GL2_MATERIAL_IMP_H

#include "include_opengl2.h"

#include <material/Material.h>

#include <graphics/graphics_define.h>


using namespace gr;

namespace higraphics {
    CLASS_BEGIN_N(GL2MaterialIMP, MaterialIMP)
private:
    GLuint program;
    
    GLint position_idx;
    GLuint translate_id;
    
    GLuint *uniform_indexes;
    
    void loadShader();
    void updateUniformValues();
    
    void bindAttributes();
    void configureUniforms();
    
    int indexOf(const pointer_vector &arr, const StringName &name);
    
protected:
    virtual void cleanPosition();
    virtual void cleanTranslate();
    
    virtual void updateShader(const Shader *shader);
    
    virtual void process();
    
public:
    
    GLint getAttributeIdx(const StringName &name);
    
    //GLint getPositionIdx();
    GLuint getTranslateID();
    
    GL2MaterialIMP();
    ~GL2MaterialIMP();
    
    CLASS_END
}

#endif //HIGRAPHICS_GL2_MATERIAL_IMP_H
