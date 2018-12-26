//
//  TextMesh.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "CharacterMesh.h"

using namespace gr;
using namespace std;

void CharacterMesh::render() {

    const Vector2f *uv = character->getUV();
    Vector2i corner = character->getCorner();
    push(Vector3f((float)corner.x() + (float)character->getWidth(), (float)corner.y() + (float)character->getHeight(), 0));
    push(Vector3f((float)corner.x() + (float)character->getWidth(), (float)corner.y(), 0));
    push(Vector3f((float)corner.x(), (float)corner.y(), 0));
    push(Vector3f((float)corner.x(), (float)corner.y() + (float)character->getHeight(), 0));
    
    vector<int> idx{
        0, 2, 1,
        2, 0, 3
    };
    commit(idx);
    
    AttrVector *uv_attr = getAttribute(Mesh::DEFAULT_UV1_NAME);
    uv_attr->setVector2f(uv[1], 0);
    uv_attr->setVector2f(Vector2f(uv[1].x(), uv[0].y()), 1);
    uv_attr->setVector2f(uv[0], 2);
    uv_attr->setVector2f(Vector2f(uv[0].x(), uv[1].y()), 3);
    
    AttrVector *color_attr = getAttribute(Mesh::DEFAULT_COLOR_NAME);
    color_attr->setVector4f(color, 0);
    color_attr->setVector4f(color, 1);
    color_attr->setVector4f(color, 2);
    color_attr->setVector4f(color, 3);
}
