//
//  Ray.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "Ray.h"

using namespace hiphysics;

HPoint Ray::position(float length) const {
    return original + direction.normalize().scale(length);
}