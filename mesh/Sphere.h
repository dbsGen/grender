//
// Created by everdom on 2017/3/2.
//

#ifndef HIRENDER_SPHERE_H
#define HIRENDER_SPHERE_H

#include "Mesh.h"
#include <math.h>
#include <render_define.h>


namespace hirender{
    CLASS_BEGIN_N(Sphere, Mesh)
public:
    enum UVType{
        UV_NORMAL,
        UV_ARGULAR
    };
private:
        float radius;
        float angleSpan;
        UVType uvType;

        void updateVertexes();
        void createVertexes();
        void updateUV();
        _FORCE_INLINE_ Vector2f getUV(const Vector3f &coord);
        _FORCE_INLINE_ const float a2r(float angle){return angle * M_PI / 180.0;}
public:
        Sphere();
        INITIALIZE(Sphere, PARAMS(float radius, float angleSpan, UVType uvType),
                   this->radius= radius;
                   this->angleSpan = angleSpan;
                   this->uvType = uvType;
                   updateVertexes();
        )

        _FORCE_INLINE_ const float getRadius(){return this->radius;}
        _FORCE_INLINE_ void setRadius(float radius){this->radius = radius; updateVertexes();}
        _FORCE_INLINE_ const float getAngleSpan(){return this->angleSpan;}
        _FORCE_INLINE_ void setAngleSpan(float angleSpan){this->angleSpan = angleSpan; updateVertexes();}
        _FORCE_INLINE_ void setUVType(UVType uvType){this->uvType = uvType; updateUV();}
    CLASS_END
}

#endif //HIRENDER_SPHERE_H
