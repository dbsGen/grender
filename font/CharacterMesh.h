//
//  TextMesh.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_TEXT_MESH_H
#define HIRENDER_TEXT_MESH_H

#include <mesh/Mesh.h>
#include "Character.h"
#include "Font.h"
#include "../render_define.h"

namespace gr {
    CLASS_BEGIN_N(CharacterMesh, Mesh)
    
private:
    Ref<Character> character;
    Vector4f color;
    
public:
    _FORCE_INLINE_ CharacterMesh() {
        color = Vector4f(1, 1, 1, 1);
    }
    
    _FORCE_INLINE_ const Ref<Character> &getCharacter(){
        return character;
    }
    _FORCE_INLINE_ void setCharacter(const Ref<Character> &ch) {
        character = ch;
    }
    
    _FORCE_INLINE_ const Vector4f &getColor() {
        return color;
    }
    _FORCE_INLINE_ void setColor(const Vector4f &color) {
        this->color = color;
    }
    
    void render();
    
    CLASS_END
}

#endif /* HIRENDER_TEXT_MESH_H */
