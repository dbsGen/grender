//
//  Panel.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/2.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "Panel.h"

using namespace gr;
using namespace std;
using namespace gc;

Panel::Panel() : Mesh(), anchor(0.5,0.5) {
    size.x(1);
    size.y(1);
    createVertexes();
}

void Panel::createVertexes() {
    push(Vector3f( size.x()*(1-anchor.x()), size.y()*(anchor.y() - 1), 0)/*, Vector2f(1, 1)*/);
    push(Vector3f( size.x()*(1-anchor.x()), size.y()*anchor.y(), 0)/*, Vector2f(1, 0)*/);
    push(Vector3f( - size.x()*anchor.x(),   size.y()*anchor.y(), 0)/*, Vector2f(0, 0)*/);
    push(Vector3f( - size.x()*anchor.x(),   size.y()*(anchor.y() - 1), 0)/*, Vector2f(0, 1)*/);
    vector<int> idx{
        0, 2, 1,
        2, 0, 3
    };
    commit(idx);
    
    AttrVector *uv_attr = getAttribute(Mesh::DEFAULT_UV1_NAME);
    uv_attr->setVector2f(Vector2f(1, 1), 0);
    uv_attr->setVector2f(Vector2f(1, 0), 1);
    uv_attr->setVector2f(Vector2f(0, 0), 2);
    uv_attr->setVector2f(Vector2f(0, 1), 3);
}

void Panel::updateVertexes() {
    AttrVector *attr = getAttribute(getVertexName());
    attr->setVector3f(Vector3f( size.x()*(1-anchor.x()), size.y()*(anchor.y() - 1), 0), 0);
    attr->setVector3f(Vector3f( size.x()*(1-anchor.x()), size.y()*anchor.y(), 0) , 1);
    attr->setVector3f(Vector3f( - size.x()*anchor.x(),   size.y()*anchor.y(), 0) , 2);
    attr->setVector3f(Vector3f( - size.x()*anchor.x(),   size.y()*(anchor.y() - 1), 0) , 3);
    attr->update();
}

void Panel::_copy(const Object *other) {
    Mesh::_copy(other);
    const Panel *om = other->cast_to<Panel>();
    if (om)
        setAnchor(om->getAnchor());
}

float Panel::getWidth() {
    return size.x();
}
void Panel::setWidth(float width) {
    if (width != size.x()) {
        size.x(width);
        updateVertexes();
    }
}

float Panel::getHeight() {
    return size.y();
}
void Panel::setHeight(float height) {
    if (height != size.y()) {
        size.y(height);
        updateVertexes();
    }
}

const Vector2f &Panel::getSize() {
    return size;
}
void Panel::setSize(Vector2f size) {
    if (size.x() != this->size.x() || size.y() != this->size.y()) {
        this->size = size;
        updateVertexes();
    }
}

void Panel::setAnchor(const Vector2f &anchor) {
    if (this->anchor != anchor) {
        this->anchor = anchor;
        updateVertexes();
    }
}
