//
// Created by hu jun on 17/5/16.
//

#include "Circle.h"

#define STEP_N 80

using namespace gr;

Circle::Circle() : Mesh(),r(1), size(Size(1,1)),anchor(CENTER) {
    createVertexes();
}

void Circle::createVertexes() {
    float step_angel = 2*M_PI/STEP_N;

    Vector3f center;
    if(anchor==CENTER)center = Vector3f(0,0,0);
    else center = Vector3f(1,1,0).scale(-r/2);

    std::vector<int> idx;
    AttrVector *uv_attr = getAttribute(Mesh::DEFAULT_UV1_NAME);

    push(center);
    uv_attr->setVector2f(Vector2f(0.5,0.5),0);

    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;
        push(Vector3f(r*cosf(angel),r*sinf(angel),0)+center);
        idx.push_back(0);
        idx.push_back(i+1);
        if(i!=STEP_N-1)
            idx.push_back(i+2);
        else
            idx.push_back(1);

        uv_attr->setVector2f(Vector2f(cosf(angel)+1,sinf(angel)+1).scale(0.5),i+1);
    }

    commit(idx);


}

void Circle::updateVertexes() {
    float step_angel = 2*M_PI/STEP_N;

    Vector3f center;
    if(anchor==CENTER)center = Vector3f(0,0,0);
    else center = Vector3f(1,1,0).scale(-r/2);


    setVertex(center,0);

    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;
        setVertex(Vector3f(r*cosf(angel),r*sinf(angel),0)+center,i+1);
    }
}

void Circle::setRadius(float radius) {
    setSize(Size(radius,radius).scale(2));
}

void Circle::setSize(const Vector2f &size) {
    if (size.x() != this->size.x() || size.y() != this->size.y()) {
        this->size = size;
        this->r = size.x()/2;
        updateVertexes();
    }
}

void Circle::setAnchor(Anchor anchor) {
    if (this->anchor != anchor) {
        this->anchor = anchor;
        updateVertexes();
    }
}
