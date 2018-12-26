//
//  Ray.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HI_PHYSICS_RAY_H
#define HI_PHYSICS_RAY_H

#include "../math/Type.h"
#include <object/Object3D.h>
#include "physics_define.h"

namespace gr {

    CLASS_BEGIN_0_N(Ray)

    private:
        Point original, direction;
        Mask hitMask;

    public:
        _FORCE_INLINE_ void setOriginal(const Point &ori) { original = ori; }
        _FORCE_INLINE_ const Point &getOriginal() const { return original; }

        _FORCE_INLINE_ void setDirection(const Point &dir) { direction = dir.normalize(); }
        _FORCE_INLINE_ const Point &getDirection() const { return direction; }

        _FORCE_INLINE_ void setHitMask(Mask hitMask) {this->hitMask = hitMask;}
        _FORCE_INLINE_ Mask getHitMask() { return hitMask; }

        Point position(float length) const;

        Ray():hitMask(0){}
        INITIALIZE(Ray, PARAMS(const Point &original, const Point &direction) ,
                   this->original = original;
                           this->direction = direction.normalize();
        )
    
        SUPPORT_VARIANT(Ray)

    CLASS_END
}

#endif /* HI_PHYSICS_RAY_H */
