//
//  PhysicsServer.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "PhysicsServer.h"
#include <core/math/Math.hpp>
#include <object/ui/View.h>
#ifdef __ANDROID__
#include <bits/stl_algo.h>

#endif

using namespace hiphysics;



bool IntersectTriangle(const Vector3f& orig, const Vector3f& dir,
                       const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
                       float* t, float* u, float* v)
{
    Vector3f E1 = v1 - v0;
    Vector3f E2 = v2 - v0;
    Vector3f P = dir.cross(E2);
    
    // determinant
    float det = E1.dot(P);
    
    // keep det > 0, modify T accordingly
    Vector3f T;
    if( det >0 )
    {
        T = orig - v0;
    }
    else
    {
        T = v0 - orig;
        det = -det;
    }
    
    // If determinant is near zero, ray lies in plane of triangle
    if( det < 0.0001f )
        return false;
    
    // Calculate u and make sure u <= 1
    *u = T.dot(P);
    if( *u < 0.0f || *u > det )
        return false;
    
    // Q
    Vector3f Q = T.cross(E1);
    
    // Calculate v and make sure u + v <= 1
    *v = dir.dot(Q);
    if( *v < 0.0f || *u + *v > det )
        return false;
    
    // Calculate t, scale parameters, ray intersects triangle
    *t = E2.dot(Q);
    
    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;
    
    return true;
}

bool Result::operator<(const hiphysics::Result &other) const {
    if (fabsf((weight - other.weight)) < 0.1) {
        View *v1 = target->cast_to<View>();
        View *v2 = other.target->cast_to<View>();
        if (v1 && !v2) {
            return true;
        }else if (!v1 && v2) {
            return false;
        }else if (v1 && v2){
            return v1->getFinalDepth() > v2->getFinalDepth();
        }
    }
    return weight < other.weight;
}

bool PhysicsServer::castObject(const Ray &ray, const Ref<Object> &object, Result &ret) {
    if (!object->isFinalEnable()) return false;
    Ref<Mesh> mesh = object->getMesh();
    const Matrix4 pose = object->getGlobalPose();
    const b8_vector &indexes = mesh->getIndexes();
    const b8_vector &vertexes = mesh->getVertexes();
    const Vector3f &original = ray.getOriginal();
    const Vector3f &direction = ray.getDirection();
    float t, u, v;
    int offset = 0;
    vector<float> crosses;
    
    ;
#define getIndex(N) *((int*)indexes.data() + (N))
    while (offset + 3 <= mesh->getIndexesCount()) {

        Vector3f v1 = Vector3f((float *)(vertexes.data() + getIndex(offset) * 3 * sizeof(float)));
        Vector3f v2 = Vector3f((float *)(vertexes.data() + getIndex(offset+1) * 3 * sizeof(float)));
        Vector3f v3 = Vector3f((float *)(vertexes.data() + getIndex(offset+2) * 3 * sizeof(float)));
        if (IntersectTriangle(original, direction,
                              v1 * pose, v2 * pose, v3 * pose,
                              &t, &u, &v)) {
            crosses.push_back(t);
        }
        
        offset+=3;
    }
    if (crosses.size()) {
        float min = LIST_MIN(crosses);
        ret.collider = Variant(ray);
        ret.position = ray.position(min);
        ret.target = object;
        ret.weight = min;
        return true;
        
    }else {
        return false;
    }
}

void PhysicsServer::add(hirender::Object *object) {
    auto it = objects.begin();
    for (auto _e = objects.end(); it != _e; ++it) {
        if (**it == object) {
            return;
        }
    }
    objects.push_back(Ref<Object>(object));
}

void PhysicsServer::remove(hirender::Object *object) {
    for (auto it = objects.begin(), _e = objects.end(); it != _e; ++it) {
        if (**it == object) objects.erase(it);
    }
}

void PhysicsServer::clear() {
    objects.clear();
}

bool PhysicsServer::cast(const Ray &ray, Result &result) {
    vector<Result> results = castAll(ray);
    if (results.size()) {
        sort(results.begin(), results.end());
        result = results.front();
        return true;
    }
    return false;
}

vector<Result> PhysicsServer::castAll(const Ray &ray) {
    vector<Result> ret;
    for (auto it = objects.begin(), _e = objects.end(); it != _e; ++it) {
        Result result;
        if (castObject(ray, *it, result)) {
            ret.push_back(result);
        }
    }
    return ret;
}
