//
//  PhysicsServer.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HI_PHYSICS_PHYSICS_SERVER_H
#define HI_PHYSICS_PHYSICS_SERVER_H

#include <object/Object3D.h>
#include "Ray.h"
#include "physics_define.h"

namespace gr {
    class PhysicsServer;

    CLASS_BEGIN_0_N(Result)

    private:
        Vector3f position;
        gc::Variant collider;
        gc::Ref<Object3D> target;
        float weight;

        friend class PhysicsServer;
    public:
        _FORCE_INLINE_ Vector3f getPosition() { return position; }
        _FORCE_INLINE_ const gc::Variant &getCollider() { return collider; }
        _FORCE_INLINE_ const gc::Ref<Object3D> &getTarget() { return target; }
        _FORCE_INLINE_ float getWeight() { return weight; }

        bool operator<(const Result &other) const;

        Result(){}

    CLASS_END

    CLASS_BEGIN_TN(PhysicsServer, gc::Singleton, 1, PhysicsServer)

    private:
        std::vector<gc::Ref<Object3D> > objects;

        static bool castObject(const Ray &ray, const gc::Ref<Object3D> &object, Result &ret);

    public:
        void add(Object3D *object);
        void remove(Object3D *object);
        void clear();

        bool cast(const Ray &ray, Result &result);
        std::vector<Result> castAll(const Ray &ray);

    CLASS_END
}

#endif /* HI_PHYSICS_PHYSICS_SERVER_H */
