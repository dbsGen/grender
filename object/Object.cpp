//
// Created by gen on 16/5/30.
//

#include "Object.h"
#include "Renderer.h"
#include "../utils/NotificationCenter.h"
#include <physics/PhysicsServer.h>

using namespace hirender;
using namespace hiphysics;

class ObjectNotifications {
public:
    static const StringName _MESSAGE_ENABLE_CHANGE;
};

const StringName ObjectNotifications::_MESSAGE_ENABLE_CHANGE("M_OBJECT_EG");
const StringName Object::MESSAGE_ENABLE_CHANGE("M_OBJECT_ENABLE_CHANGE");

const StringName Object::MESSAGE_DISPLAY_CHANGED("M_OBJECT_DISPLAY_CHANGED");

const StringName Object::MESSAGE_MASK_CHANGE("M_OBJECT_MASK_CHANGE");
const StringName Object::MESSAGE_HIT_MASK_CHANGE("M_OBJECT_HIT_MASK_CHANGE");
const StringName Object::MESSAGE_ADD_CHILD("M_OBJECT_ADD_CHILD");
const StringName Object::MESSAGE_REMOVE_CHILD("M_OBJECT_REMOVE_CHILD");
const StringName Object::MESSAGE_CHANGE_MATERIAL("M_OBJECT_CHANGE_MATERIAL");
const StringName Object::MESSAGE_UPDATE_POSE("M_OBJECT_UPDATE_POSE");
const StringName Object::MESSAGE_TOUCH_EVENT("M_OBJECT_TOUCH_EVENT");

Object::Object() : pose(Matrix4::identity()),
                   awake_count(0),
                   collision(false),
                   dirty_global_pose(true),
                   enable(true),
                   dirty_enable(true),
                   update_enable(false),
                   step_enable(false),
                   single(false),
                   _static(false),
                   update_type(0),
                   renderer(NULL) {
    parent = NULL;
    hitMask = 0;
    mask = 1;
                       _scale = Vector3f::one();
}

Object::~Object() {
    if (renderer) {
        renderer->cancelDoOnMainThread(this);
        renderer = nullptr;
        if (getCollition()) {
            PhysicsServer::getInstance()->remove(this);
            collision = false;
        }
    }
    setUpdateEnable(false);
    setStepEnable(false);
}

void Object::add(const Ref<Object> &object) {
    if (*object == this) {
        LOG(e, "Can not add child with self");
        return;
    }
    Object *obj = *object;
    if (!obj) {
        LOG(e, "Object is null.");
        return;
    }
    Object *parent = obj->getParent();
    if (parent != this) {
        if (parent) {
            parent->remove(object);
        }
        children.push_back(object);
        object->parent = this;
        obj->dirty_global_pose = true;
        obj->dirty_enable = true;
        obj->change();
        _addChild(object, this);
    }
}

void Object::remove(const Ref<Object> &object) {
    Object *parent = object->getParent();
    if (parent == this) {
        children.remove(object);
        object->parent = NULL;
        _removeChild(object, this);
    }
}

void Object::_addChild(const Ref<Object> &child, Object *parent) {
    vector<Variant> vs{Variant(child), Variant(parent)};
    Array arr(vs);
    sendMessage(MESSAGE_ADD_CHILD, UP, &arr);
}

void Object::_removeChild(const Ref<Object> &child, Object *parent) {
    vector<Variant> vs{Variant(child), Variant(parent)};
    Array arr(vs);
    sendMessage(MESSAGE_REMOVE_CHILD, UP, &arr);
}

void Object::_changeMaterial(Object *object, Material *mat) {
    vector<Variant> vs{Variant(object), Variant(mat)};
    Array arr(vs);
    sendMessage(MESSAGE_CHANGE_MATERIAL, UP, &arr);
    change();
}

void Object::_updatePose() {
    vector<Variant> vs{Variant(this)};
    Array arr(vs);
    sendMessage(MESSAGE_UPDATE_POSE, DOWN, &arr);
    onPoseChanged();
    change();
}

void Object::updateTRS() {
    position = pose.position();
    rotation = pose.rotation();
    _scale = pose.scale();
}

void Object::setPose(const Matrix4 &pose) {
    this->pose = pose;
    updateTRS();
    _updatePose();
}

void Object::rotate(float radias, const Vector3f &axis)  {
    pose = pose.rotate(radias, axis);
    updateTRS();
    _updatePose();
}

void Object::translate(const Vector3f &trans) {
    pose = pose + trans;
    updateTRS();
    _updatePose();
}

void Object::scale(const Vector3f &scale) {
    pose = pose * scale;
    updateTRS();
    _updatePose();
}

void Object::setPosition(const Vector3f &pos) {
    if (position != pos) {
        position = pos;
        pose = Matrix4::TRS(position, rotation, _scale);
        _updatePose();
    }
}

void Object::setRotation(const HQuaternion &rot) {
    if (rotation != rot) {
        rotation = rot;
        pose = Matrix4::TRS(position, rotation, _scale);
        _updatePose();
    }
}

void Object::setScale(const Vector3f &scale) {
    if (_scale != scale) {
        _scale = scale;
        pose = Matrix4::TRS(position, rotation, _scale);
        _updatePose();
    }
}

void Object::_added(Renderer *renderer) {
    this->renderer = renderer;
    if (awake_count == 0) {
        awake(renderer);
        if (getCollition()) {
            PhysicsServer::getInstance()->add(this);
        }
    }
    ++awake_count;
}

void Object::_removed(Renderer *renderer) {
    if (awake_count > 0) {
        --awake_count;
        if (awake_count == 0) {
            if (renderer) {
                renderer->cancelDoOnMainThread(this);
            }
            if (getCollition()) {
                PhysicsServer::getInstance()->remove(this);
            }
            this->renderer = nullptr;
        }
    }
}

void Object::_touchBegin(Camera *renderer, const Vector2f &screenPoint) {
    vector<Variant> vs{
        (int)TOUCH_BEGIN,
        screenPoint
    };
    Array arr(vs);
    sendMessage(MESSAGE_TOUCH_EVENT,
                UP,
                &arr);
}
void Object::_touchMove(Camera *renderer, bool inside, const Vector2f &screenPoint) {
    vector<Variant> vs{
        (int)TOUCH_MOVE,
        screenPoint
    };
    Array arr(vs);
    sendMessage(MESSAGE_TOUCH_EVENT,
                UP,
                &arr);
}
void Object::_touchEnd(Camera *renderer, const Vector2f &screenPoint) {
    vector<Variant> vs{
        (int)TOUCH_END,
        screenPoint
    };
    Array arr(vs);
    sendMessage(MESSAGE_TOUCH_EVENT,
                UP,
                &arr);
}

void Object::_touchCancel(Camera *renderer, const Vector2f &screenPoint) {
    vector<Variant> vs{
        (int)TOUCH_CANCEL,
        screenPoint
    };
    Array arr(vs);
    sendMessage(MESSAGE_TOUCH_EVENT,
                UP,
                &arr);
}
void Object::_touchMoveIn(Camera *renderer, const Vector2f &screenPoint) {
    vector<Variant> vs{
        (int)TOUCH_MOVE_IN,
        screenPoint
    };
    Array arr(vs);
    sendMessage(MESSAGE_TOUCH_EVENT,
                UP,
                &arr);
}
void Object::_touchMoveOut(Camera *renderer, const Vector2f &screenPoint) {
    vector<Variant> vs{
        (int)TOUCH_MOVE_OUT,
        screenPoint
    };
    Array arr(vs);
    sendMessage(MESSAGE_TOUCH_EVENT,
                UP,
                &arr);
}

bool Object::onMessage(const hicore::StringName &key, const Array *vars) {
    if (key == MESSAGE_UPDATE_POSE) {
        dirty_global_pose = true;
        if (Renderer::currentThread() == Renderer::MainThread)
            poseUpdateInv();
        else {
            if (renderer) {
                renderer->doOnMainThread(this, Object::mainThreadPoseUpdateInv);
            }
        }
    }else if (key == ObjectNotifications::_MESSAGE_ENABLE_CHANGE) {
        if (vars->at(0).get<Object>() != this)
            dirty_enable = true;
    }else if (key == MESSAGE_TOUCH_EVENT) {
        on_event(this, (void*)vars);
    }else if (key == MESSAGE_DISPLAY_CHANGED) {
        return isFinalEnable() ? onChanged() : false;
    }
    return true;
}

void Object::mainThreadPoseUpdateInv(void *renderer, void *object, void *data) {
    Object *obj = (Object*)object;
    obj->poseUpdateInv();
}

void Object::poseUpdateInv() {
    for (auto it = pose_update_callbacks.begin(), _e = pose_update_callbacks.end();
         it != _e; ++it) {
        ActionItem *ai = (ActionItem *)it->second;
        ai->callback(this, ai->data, it->first);
    }
}

void Object::mainTreadEnableChangedInv(void *renderer, void *object, void *data) {
    Object *obj = (Object*)object;
    obj->enableChangedInv();
}

void Object::enableChangedInv() {

}

void Object::addPoseCallback(void *target, ActionCallback callback, void *data) {
    ActionItem *ai = new ActionItem(callback, data);
    pose_update_callbacks[target] = ai;
}
void *Object::removePoseCallback(void *target) {
    auto it = pose_update_callbacks.find(target);
    if (it != pose_update_callbacks.end()) {
        ActionItem *ai = (ActionItem*)it->second;
        void *data = ai->data;
        delete ai;
        pose_update_callbacks.erase(it);
        return data;
    }
    return NULL;
}

void Object::_message(const StringName &key, NotifyDirection direction, const Array *vars) {
    if (!onMessage(key, vars)) return;
    switch (direction) {
        case UP:
        {
            Object *p = getParent();
            if (p) p->_message(key, direction, vars);
        }
            break;
        case DOWN:
        {
            const list<Ref<Object> > &children = getChildren();
            for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
                (*it)->_message(key, direction, vars);
            }
        }
            break;
            
        default:
            break;
    }
}

void Object::sendMessage(const StringName &key,
                         NotifyDirection direction,
                         const Array *vars) {
    switch (direction) {
        case BOTH: {
            _message(key, NONE, vars);
            Object *p = getParent();
            if (p) p->_message(key, UP, vars);
            const list<Ref<Object> > &children = getChildren();
            for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
                (*it)->_message(key, DOWN, vars);
            }
        }
            break;
            
        default:
            _message(key, direction, vars);
            break;
    }
}

const Matrix4 &Object::getGlobalPose() const {
    if (dirty_global_pose) {
        Object *that = const_cast<Object*>(this);
        that->global_pose = getPose();
        const Object *p = getParent();
        if (p) that->global_pose = p->getGlobalPose() * that->global_pose;
        that->dirty_global_pose = false;
    }
    return global_pose;
}

void Object::setCollision(bool collision) {
    if (this->collision != collision) {
        this->collision = collision;
        if (getParent()) {
            if (collision) {
                PhysicsServer::getInstance()->add(this);
            }else {
                PhysicsServer::getInstance()->remove(this);
            }
        }
    }
}

const Ref<Mesh> &Object::getCollider() const {
    return collider ? collider : mesh;
}

void Object::setEnable(bool enable) {
    if (this->enable != enable) {
        bool old = isFinalEnable();
        this->enable = enable;
        dirty_enable = true;
        bool n = isFinalEnable();
        if (old != n) {
            variant_vector vs{this, old, n};
            Array arr(vs);
            sendMessage(ObjectNotifications::_MESSAGE_ENABLE_CHANGE, DOWN, &arr);
            sendMessage(MESSAGE_ENABLE_CHANGE, UP, &arr);
            change();
            if (enable) {
                onEnable();
            }else {
                onDisable();
            }
        }
    }
}

bool Object::isFinalEnable() {
    if (dirty_enable) {
        dirty_enable = false;
        auto p = getParent();
        final_enable = enable && (p ? p->isFinalEnable() : true);
    }
    return final_enable;
}

void Object::onEnable() {
    const list<Ref<Object> > &children = getChildren();
    for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
        if ((*it)->getEnable()) {
            (*it)->onEnable();
        }
    }
}

void Object::onDisable() {
    const list<Ref<Object> > &children = getChildren();
    for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
        if (!(*it)->getEnable()) {
            (*it)->onDisable();
        }
    }
}

void Object::traversal(TraversalChecker checker, TraversalHandle dofun, void *data) {
    if (checker(this, data)) {
        dofun(this, data);
    }
    for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
        (*it)->traversal(checker, dofun, data);
    }
}

void Object::copyParameters(const Ref<Object> &other) {
    setEnable(other->getEnable());
    setCollider(other->getCollider());
    setCollision(other->getCollition());
    setHitMask(other->getHitMask());
    setMask(other->getMask());
    setMesh(other->getMesh());
    setMaterial(other->getMaterial());
}

void Object::setMask(Mask mask) {
    int old = this->mask;
    this->mask = mask;
    vector<Variant> vs{this, old, (int)mask};
    Array arr(vs);
    sendMessage(MESSAGE_MASK_CHANGE, UP, &arr);
}

void Object::setHitMask(Mask mask) {
    int old = hitMask;
    hitMask = mask;
    vector<Variant> vs{this, old, (int)mask};
    Array arr(vs);
    sendMessage(MESSAGE_HIT_MASK_CHANGE, UP, &arr);
}

void Object::updateCallback(void *key, void *params, void *data) {
    variant_vector *ps = (variant_vector*)params;
    Object *target = (Object *)data;
    Variant &delta = ps->operator[](0);
    target->update(delta);
    const Variant *vs[1];
    vs[0] = &delta;
    target->apply("_update", NULL, vs, 1);
//    if (target->update_type == 2 || target->update_type == 3) {
//        target->_poseUpdateInv();
//        if (target->update_type == 2) {
//            target->setUpdateEnable(false);
//        }else {
//            target->update_type = 1;
//        }
//    }
}

void Object::stepCallback(void *key, void *params, void *data) {
    variant_vector *ps = (variant_vector*)params;
    Object *target = (Object *)data;
    Variant &delta = ps->operator[](0);
    target->step(delta);
    const Variant *vs[1];
    vs[0] = &delta;
    target->apply("_step", NULL, vs, 1);
}

void Object::setUpdateEnable(bool enable) {
    if (!update_enable && enable) {
        if (update_enable == 0 || update_enable == 2) update_type = 1;
        update_enable = enable;
        if (notification_key.empty()) notification_key = NotificationCenter::keyFromObject(this);
        NotificationCenter::getInstance()->listen(Renderer::NOTIFICATION_FRAME_STEP, updateCallback, notification_key, this);
    }else if (update_enable && !enable) {
        update_type = 0;
        update_enable = enable;
        if (notification_key.empty()) notification_key = NotificationCenter::keyFromObject(this);
        NotificationCenter::getInstance()->remove(Renderer::NOTIFICATION_FRAME_STEP, notification_key);
    }
}

void Object::setStepEnable(bool enable) {
    if (!step_enable && enable) {
        step_enable = enable;
        if (notification_key.empty()) notification_key = NotificationCenter::keyFromObject(this);
        NotificationCenter::getInstance()->listen(Renderer::NOTIFICATION_FIXED_UPDATE, stepCallback, notification_key, this);
    }else if (step_enable && !enable) {
        step_enable = enable;
        if (notification_key.empty()) notification_key = NotificationCenter::keyFromObject(this);
        NotificationCenter::getInstance()->remove(Renderer::NOTIFICATION_FIXED_UPDATE, notification_key);
    }
}

void Object::change() {
    sendMessage(MESSAGE_DISPLAY_CHANGED, UP);
}

