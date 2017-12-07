//
//  PhysicsServer.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HI_PHYSICS_PHYSICS_SERVER_H
#define HI_PHYSICS_PHYSICS_SERVER_H

#include <object/Object.h>
#include "Ray.h"
#include "physics_define.h"

using namespace hicore;
using namespace hirender;

namespace hiphysics {
    class PhysicsServer;

    CLASS_BEGIN_0_N(Result)

    private:
        Vector3f position;
        Variant collider;
        Ref<Object> target;
        float weight;

        friend class PhysicsServer;
    public:
        _FORCE_INLINE_ Vector3f getPosition() { return position; }
        _FORCE_INLINE_ const Variant &getCollider() { return collider; }
        _FORCE_INLINE_ const Ref<Object> &getTarget() { return target; }
        _FORCE_INLINE_ float getWeight() { return weight; }

        bool operator<(const Result &other) const;

        Result(){}

    CLASS_END

    CLASS_BEGIN_TN(PhysicsServer, Singleton, 1, PhysicsServer)

    private:
        vector<Ref<Object> > objects;

        static bool castObject(const Ray &ray, const Ref<Object> &object, Result &ret);

    public:
        void add(Object *object);
        void remove(Object *object);
        void clear();

        bool cast(const Ray &ray, Result &result);
        vector<Result> castAll(const Ray &ray);

    CLASS_END
}

#endif /* HI_PHYSICS_PHYSICS_SERVER_H */
