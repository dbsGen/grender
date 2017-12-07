//
// Created by gen on 16/6/22.
//

#include <object/ui/View.h>
#include <core/math/Math.hpp>
#include <physics/PhysicsServer.h>
#include "Camera.h"

using namespace hirender;
using namespace hiphysics;

const Matrix4 &Camera::getProjection() {
    if (dirty_projection) {
        switch (type) {
            case Type::Orthographic: {
                float offx = (rect.v[2]) * (1-viewport.z())/2;
                float offy = (rect.v[3]) * (1-viewport.w())/2;
                projection = Matrix4::ortho(rect.v[0] + offx,
                                            rect.v[0] + offx + rect.v[2] * viewport.z(),
                                            rect.v[1] + offy,
                                            rect.v[1] + offy + rect.v[3] * viewport.w(),
                                            near, far);
            }
                break;
            case Type::Perspective: {
                HRect tr = rect * (fov / 111.88614049404633);
                float offx = (tr.v[2]) * (1-viewport.z())/2;
                float offy = (tr.v[3]) * (1-viewport.w())/2;
                projection = Matrix4::perspective(tr.v[0] + offx,
                                                  tr.v[0] + offx + tr.v[2] * viewport.z(),
                                                  tr.v[1] + offy,
                                                  tr.v[1] + offy + tr.v[3] * viewport.w(),
                                                  near, far);
            }
                break;
        }
        dirty_projection = false;
    }
    return projection;
}

HPoint Camera::screenToWorld(const HPoint &screen_point) {
    return HPoint(2*screen_point.x()-1, 1-2*screen_point.y(), screen_point.z()) * getProjection().inverse();
}

HPoint Camera::worldToScreen(const HPoint &world_point) {
    Vector4f v4(world_point.x(), world_point.y(), world_point.z(), 1);
    Vector4f point = v4 * (getProjection() * getGlobalPose().inverse());
    point *= 1/point.w();
    return Vector3f((point.x() + 1) / 2, (1 - point.y()) / 2, point.z());
}

bool Camera::sortCompare(Object *o1, Object *o2) {
    const HClass *c1 = o1->getInstanceClass();
    const HClass *c2 = o2->getInstanceClass();
    const HClass *vc = View::getClass();
    if (c1->isTypeOf(vc)) {
        if (c2->isTypeOf(vc)) {
            View *v1 = o1->cast_to<View>();
            View *v2 = o2->cast_to<View>();
            return v1->getFinalDepth() < v2->getFinalDepth();
        }else {
            return false;
        }
    }else if (c2->isTypeOf(vc)) {
        return true;
    }else {
        const Ref<Material> &m1 = o1->getMaterial();
        const Ref<Material> &m2 = o2->getMaterial();
        if (m1 && !m2) {
            return true;
        }else if (!m1 && m2) {
            return false;
        }else if (m1) {
            return *m1 < *m2;
        }
        return false;
    }
}

void Camera::copyParameters(const Ref<Object> &other) {
    Object::copyParameters(other);
    Camera *camera = other->cast_to<Camera>();
    if (camera) {
        setDepth(camera->getDepth());
        setFar(camera->getFar());
        setNear(camera->getNear());
        setViewport(camera->getViewport());
        setRect(camera->getRect());
        setType(camera->getType());
        setProjection(camera->getProjection());
        setFov(camera->getFov());
    }
}

bool Camera::onMessage(const StringName &key, const Array *vars) {
    return Object::onMessage(key, vars);
}

void Camera::sendTouchEvent(Object::EventType event, const Vector2f &point) {
    if (!viewport.contain(point)) {
        if (current_touch_object) {
//                if (focus)
            current_touch_object->_touchCancel(this, point);
            current_touch_object = NULL;
        }
        focus = false;
        return;
    }
    Ray ray = rayFromScreenPoint(point);
    ray.setHitMask(getHitMask());
    switch (event) {
        case Object::TOUCH_BEGIN: {
            Result result;
            if (PhysicsServer::getInstance()->cast(ray, result)) {
                current_touch_object = result.getTarget();
                current_touch_object->_touchBegin(this, point);
                focus = true;
            }
            break;
        }
        case Object::TOUCH_MOVE: {
            if (current_touch_object) {
                Result result;
                if (PhysicsServer::getInstance()->cast(ray, result)) {
                    if (current_touch_object == result.getTarget()) {
                        if (focus) {
                        }else {
                            current_touch_object->_touchMoveIn(this, point);
                            focus = true;
                        }
                        current_touch_object->_touchMove(this, true, point);
                        break;
                    }
                }
                if (focus && current_touch_object) {
                    current_touch_object->_touchMoveOut(this, point);
                    focus = false;
                }
                current_touch_object->_touchMove(this, false, point);
            }
            break;
        }
        case Object::TOUCH_END: {
            if (current_touch_object) {
//                if (focus)
                    current_touch_object->_touchEnd(this, point);
                current_touch_object = NULL;
            }
            focus = false;
            break;
        }
        case Object::TOUCH_CANCEL: {
            if (current_touch_object) {
//                if (focus)
                    current_touch_object->_touchCancel(this, point);
                current_touch_object = NULL;
            }
            focus = false;
            break;
        }
        default:
            break;
    }
}

hiphysics::Ray Camera::rayFromScreenPoint(const Vector2f &point) {
    HPoint p((point.x() - viewport.left()) / viewport.width(), (point.y() - viewport.top()) / viewport.height(), 0);
    if (type == Type::Perspective) {
        HPoint po = HPoint() * getGlobalPose();
        HPoint pe = screenToWorld(p) * getGlobalPose();
        return Ray(po, pe - po);
    }else {
        HPoint po = screenToWorld(p) * getGlobalPose();
        p.z(getNear());
        HPoint pe = screenToWorld(p) * getGlobalPose();
        return Ray(po, pe - po);
    }
}
