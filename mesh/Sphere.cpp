//
// Created by everdom on 2017/3/2.
//

#include "Sphere.h"

using namespace hirender;

Sphere::Sphere(): Mesh(), radius(1), uvType(UV_NORMAL), angleSpan(10.0f){
    createVertexes();
}

void Sphere::updateVertexes() {
    float angleA, angleB;
    int idx = 0;

    // calculate coordinate on a sphere surface
    for (angleA = -90.0f; angleA <= 90.0f; angleA += this->angleSpan) {
        for(angleB = 0.0f; angleB < 360.0f; angleB += this->angleSpan){
            float x = this->radius * cosf(a2r(angleA)) * sinf(a2r(angleB));
            float y = this->radius * sinf(a2r(angleA));
            float z = this->radius * cosf(a2r(angleA)) * cosf(a2r(angleB));
            setVertex(Vector3f(x, y, z), idx++);
        }
    }
}

/**
x0 = R * cos(a) * sin(b);
y0 = R * sin(a);
z0 = R * cos(a) * cos(b);
 */
void Sphere::createVertexes() {
    float angleA, angleB;
    int i;

    AttrVector *uv = getAttribute(Mesh::DEFAULT_UV1_NAME);
    // calculate coordinate on a sphere surface
    for (i = 0, angleA = -90.0f; angleA <= 90.0f; angleA += this->angleSpan) {
        for(angleB = 0.0f; angleB < 360.0f; angleB += this->angleSpan){
            float x = this->radius * cosf(a2r(angleA)) * sinf(a2r(angleB));
            float y = this->radius * sinf(a2r(angleA));
            float z = this->radius * cosf(a2r(angleA)) * cosf(a2r(angleB));
            push(Vector3f(x, y, z));

            uv->setVector2f(this->getUV(Vector3f(x, y, z)), i++);
        }
    }

    // combile vertex into trianlge plane
    int row = (180 / this->angleSpan) + 1;
    int col = 360 / this->angleSpan;
    vector<int> idx;

    for(int i = 0; i < row; i++){
        if(i>0 && i<row-1){
            for (int j = -1; j < col; j++) {
                int k=i*col+j;
                idx.push_back(k+col);
                idx.push_back(k+1);
                idx.push_back(k);
            }
            for (int j = 0; j < col+1; j++) {
                int k=i*col+j;
                idx.push_back(k-col);
                idx.push_back(k-1);
                idx.push_back(k);
            }
        }
    }
    commit(idx);
}

void Sphere::updateUV() {
    float angleA, angleB;
    int i;

    AttrVector *uv = getAttribute(Mesh::DEFAULT_UV1_NAME);
    for (i = 0, angleA = -90.0f; angleA <= 90.0f; angleA += this->angleSpan) {
        for (angleB = 0.0f; angleB < 360.0f; angleB += this->angleSpan) {
            float x = this->radius * cosf(a2r(angleA)) * sinf(a2r(angleB));
            float y = this->radius * sinf(a2r(angleA));
            float z = this->radius * cosf(a2r(angleA)) * cosf(a2r(angleB));

            uv->setVector2f(this->getUV(Vector3f(x, y, z)), i++);
        }
    }
}

/*
NORMAL: U=arctan(x/z)/2/PI+0.5, V=arcsin(y)/PI+0.5
ARGULAR: m = 2*sqrt(x*x+y*y+(z+1)*(z+1)); U = x/m + 0.5; V = y/m + 0.5;
 */
Vector2f Sphere::getUV(const Vector3f &coord){
    Vector2f uv;
    switch(this->uvType){
        case UV_NORMAL:
        default:
            uv.x(atan2f(coord.x(), coord.z())/2.0f/M_PI+0.5);
            uv.y(1-(asinf(coord.y())/M_PI+0.5));
        break;
        case UV_ARGULAR:
        {
            float m = 2 * sqrt(coord.x() * coord.x() + coord.y() * coord.y() +
                               (coord.z() + 1) * (coord.z() + 1));
            uv.x(coord.x() / m + 0.5);
            uv.y(1-(coord.y() / m + 0.5));
        }
        break;
    }
    return uv;
}
