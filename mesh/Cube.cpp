//
//  Cube.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/13.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "Cube.h"

using namespace hirender;

Cube::Cube() {
    size.x(1);
    size.y(1);
    size.z(1);
    createVertexes();
    
}

void Cube::updateVertexes() {
    setVertex(Vector3f( size.x()/2, -size.y()/2,  size.z()/2), 0);
    setVertex(Vector3f( size.x()/2,  size.y()/2,  size.z()/2), 1);
    setVertex(Vector3f(-size.x()/2,  size.y()/2,  size.z()/2), 2);
    setVertex(Vector3f(-size.x()/2, -size.y()/2,  size.z()/2), 3);
    
    setVertex(Vector3f( size.x()/2, -size.y()/2, -size.z()/2), 4);
    setVertex(Vector3f( size.x()/2,  size.y()/2, -size.z()/2), 5);
    setVertex(Vector3f(-size.x()/2,  size.y()/2, -size.z()/2), 6);
    setVertex(Vector3f(-size.x()/2, -size.y()/2, -size.z()/2), 7);
    
    setVertex(Vector3f(-size.x()/2, -size.y()/2, -size.z()/2), 8);
    setVertex(Vector3f(-size.x()/2, -size.y()/2,  size.z()/2), 9);
    setVertex(Vector3f(-size.x()/2,  size.y()/2,  size.z()/2), 10);
    setVertex(Vector3f(-size.x()/2,  size.y()/2, -size.z()/2), 11);
    
    setVertex(Vector3f( size.x()/2, -size.y()/2,  size.z()/2), 12);
    setVertex(Vector3f( size.x()/2, -size.y()/2, -size.z()/2), 13);
    setVertex(Vector3f( size.x()/2,  size.y()/2, -size.z()/2), 14);
    setVertex(Vector3f( size.x()/2,  size.y()/2,  size.z()/2), 15);
}

void Cube::createVertexes() {
    push(Vector3f( size.x()/2, -size.y()/2,  size.z()/2)/*, Vector2f(1, 1)*/);
    push(Vector3f( size.x()/2,  size.y()/2,  size.z()/2)/*, Vector2f(1, 0)*/);
    push(Vector3f(-size.x()/2,  size.y()/2,  size.z()/2)/*, Vector2f(0, 0)*/);
    push(Vector3f(-size.x()/2, -size.y()/2,  size.z()/2)/*, Vector2f(0, 1)*/);
    
    push(Vector3f( size.x()/2, -size.y()/2, -size.z()/2)/*, Vector2f(1, 0)*/);
    push(Vector3f( size.x()/2,  size.y()/2, -size.z()/2)/*, Vector2f(1, 1)*/);
    push(Vector3f(-size.x()/2,  size.y()/2, -size.z()/2)/*, Vector2f(0, 1)*/);
    push(Vector3f(-size.x()/2, -size.y()/2, -size.z()/2)/*, Vector2f(0, 0)*/);
    
    push(Vector3f(-size.x()/2, -size.y()/2, -size.z()/2)/*, Vector2f(1, 1)*/);
    push(Vector3f(-size.x()/2, -size.y()/2,  size.z()/2)/*, Vector2f(0, 1)*/);
    push(Vector3f(-size.x()/2,  size.y()/2,  size.z()/2)/*, Vector2f(0, 0)*/);
    push(Vector3f(-size.x()/2,  size.y()/2, -size.z()/2)/*, Vector2f(1, 0)*/);
    
    push(Vector3f( size.x()/2, -size.y()/2,  size.z()/2)/*, Vector2f(1, 1)*/);
    push(Vector3f( size.x()/2, -size.y()/2, -size.z()/2)/*, Vector2f(0, 1)*/);
    push(Vector3f( size.x()/2,  size.y()/2, -size.z()/2)/*, Vector2f(0, 0)*/);
    push(Vector3f( size.x()/2,  size.y()/2,  size.z()/2)/*, Vector2f(1, 0)*/);
    vector<int> idx{
        0, 2, 1,
        0, 3, 2,
        
        4, 3, 0,
        3, 4, 7,
        
        1, 6, 5,
        1, 2, 6,
        
        4, 6, 7,
        4, 5, 6,
        
        8,10, 9,
        8,11,10,
        
        12,14,13,
        12,15,14
    };
    commit(idx);
    
    AttrVector *uv = getAttribute(Mesh::DEFAULT_UV1_NAME);
    uv->setVector2f(Vector2f(1, 1), 0);
    uv->setVector2f(Vector2f(1, 0), 1);
    uv->setVector2f(Vector2f(0, 0), 2);
    uv->setVector2f(Vector2f(0, 1), 3);
    
    uv->setVector2f(Vector2f(1, 0), 4);
    uv->setVector2f(Vector2f(1, 1), 5);
    uv->setVector2f(Vector2f(0, 1), 6);
    uv->setVector2f(Vector2f(0, 0), 7);
    
    uv->setVector2f(Vector2f(1, 1), 8);
    uv->setVector2f(Vector2f(0, 1), 9);
    uv->setVector2f(Vector2f(0, 0), 10);
    uv->setVector2f(Vector2f(1, 0), 11);
    
    uv->setVector2f(Vector2f(1, 1), 12);
    uv->setVector2f(Vector2f(0, 1), 13);
    uv->setVector2f(Vector2f(0, 0), 14);
    uv->setVector2f(Vector2f(1, 0), 15);
    
}
