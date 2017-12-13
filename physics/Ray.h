//
//  Ray.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HI_PHYSICS_RAY_H
#define HI_PHYSICS_RAY_H

#include <core/math/Type.h>
#include <object/Object.h>
#include "physics_define.h"

using namespace gr;

namespace hiphysics {

    CLASS_BEGIN_0_N(Ray)

    private:
        HPoint original, direction;
        Mask hitMask;

    public:
        _FORCE_INLINE_ void setOriginal(const HPoint &ori) { original = ori; }
        _FORCE_INLINE_ const HPoint &getOriginal() const { return original; }

        _FORCE_INLINE_ void setDirection(const HPoint &dir) { direction = dir.normalize(); }
        _FORCE_INLINE_ const HPoint &getDirection() const { return direction; }

        _FORCE_INLINE_ void setHitMask(Mask hitMask) {this->hitMask = hitMask;}
        _FORCE_INLINE_ Mask getHitMask() { return hitMask; }

        HPoint position(float length) const;

        Ray():hitMask(0){}
        INITIALIZE(Ray, PARAMS(const HPoint &original, const HPoint &direction) ,
                   this->original = original;
                           this->direction = direction.normalize();
        )
    
        SUPPORT_VARIANT(Ray)

    CLASS_END
}

#endif /* HI_PHYSICS_RAY_H */
