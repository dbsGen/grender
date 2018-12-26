//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_OBJECT_H
#define HIRENDER_OBJECT_H

#include <material/Material.h>
#include <mesh/Mesh.h>
#include <core/Ref.h>
#include "../math/Type.h"
#include <core/Array.h>
#include <core/Object.h>

#include <render_define.h>

namespace gr {
    class Linker;
    class Renderer;
    class Camera;

    typedef int Mask;
#define MaskMask 0xFFFFFFFF
    _FORCE_INLINE_ Mask MaskAt(int i) {return (Mask) (1 << i) & MaskMask;}
#define UIMask MaskAt(31)
    _FORCE_INLINE_ Mask MaskHitAt(Mask mask, int i) {return (mask | MaskAt(i)) & MaskMask;}
    _FORCE_INLINE_ Mask MaskIgnoreAt(Mask mask, int i) {return (mask & ~MaskAt(i)) & MaskMask;}
    _FORCE_INLINE_ bool MaskHit(Mask m1, Mask m2) {return (bool) (m1 & m2);}

    CLASS_BEGIN_N(Object3D, gc::RefObject)
    public:
        enum _NotifyDirection {
            BOTH,
            UP,
            DOWN,
            NONE        // not send
        };
        typedef char NotifyDirection;
        enum _EventType {
            TOUCH_BEGIN = 0,
            TOUCH_MOVE,
            TOUCH_END,
            TOUCH_CANCEL,
            TOUCH_MOVE_IN,
            TOUCH_MOVE_OUT,
            FOCUS_IN,
            FOCUS_OUT
        };
        typedef char EventType;

    private:
        gc::Ref<Material>   material;
        gc::Ref<Mesh>       mesh;
        std::list<gc::Ref<Object3D>>   children;
        Object3D            *parent;
        Matrix4             pose;
        Matrix4             global_pose;
        bool                dirty_global_pose;

        Vector3f            position;
        Quaternion          rotation;
        Vector3f            _scale;

        int                 awake_count;
        bool                enable;
        bool                dirty_enable;
        bool                final_enable;

        int                 update_type;
        bool                update_enable;
        bool                step_enable;
        bool                single;
        bool                _static;

        bool                collision;
        gc::Ref<Mesh>       collider;

        gc::StringName      name;
        gc::StringName      notification_key;

        gc::ActionItem      on_event;
    
        pointer_map         pose_update_callbacks;
    
        Renderer            *renderer;

        Mask    mask;
        Mask    hitMask;

        void _addChild(const gc::Ref<Object3D> &child, Object3D *parent);
        void _removeChild(const gc::Ref<Object3D> &child, Object3D *parent);
        void _changeMaterial(Object3D *object, Material *old);
        void _updatePose();
        void _message(const gc::StringName &key, NotifyDirection direction, const gc::RArray *vars);

        void _added(Renderer *renderer);
        void _removed(Renderer *renderer);

        void _touchBegin(Camera *renderer, const Vector2f &screenPoint);
        void _touchMove(Camera *renderer, bool inside, const Vector2f &screenPoint);
        void _touchEnd(Camera *renderer, const Vector2f &screenPoint);
        void _touchCancel(Camera *renderer, const Vector2f &screenPoint);
        void _touchMoveIn(Camera *renderer, const Vector2f &screenPoint);
        void _touchMoveOut(Camera *renderer, const Vector2f &screenPoint);
    
        static void mainThreadPoseUpdateInv(void *renderer, void *object, void *data);
        void poseUpdateInv();

        static void mainTreadEnableChangedInv(void *renderer, void *object, void *data);
        void enableChangedInv();

        static void updateCallback(void *key, void *params, void *data);
        static void stepCallback(void *key, void *params, void *data);
        void updateTRS();

        friend class Renderer;
        friend class Camera;

    protected:
        virtual bool onMessage(const gc::StringName &key, const gc::RArray *vars);

        _FORCE_INLINE_ virtual void awake(Renderer *renderer) {}

        virtual void onEnable();
        virtual void onDisable();

        /**
         * Frame update
         */
        _FORCE_INLINE_ virtual void update(double delta) {}
        EVENT(void, _update)
        /**
         * Fixed update
         */
        _FORCE_INLINE_ virtual void step(double delta) {}
        EVENT(void, _step)

        _FORCE_INLINE_ virtual bool onChanged() { return true; }
    
        _FORCE_INLINE_ virtual void onPoseChanged() {}

        friend class MeshIMP;

    public:
        /**
         * Notifitied when changed
         * No param
         */
        static const gc::StringName MESSAGE_DISPLAY_CHANGED;
        /**
         * Notifity when mask changed, SELF
         * @param [Object* self, Mask from, Mask to]
         */
        static const gc::StringName MESSAGE_MASK_CHANGE;

        /**
         * Notifity when enable changed
         * @param [Object* self, bool old, bool new]
         */
        static const gc::StringName MESSAGE_ENABLE_CHANGE;
        /**
         * Notifity when hit mask changed, UP
         * @param [Object* self, Mask from, Mask to]
         */
        static const gc::StringName MESSAGE_HIT_MASK_CHANGE;

        /**
         * Notifity when added a child, UP
         * @param [Object* parent, Object *child]
         */
        static const gc::StringName MESSAGE_ADD_CHILD;

        /**
         * Notifity when removed a child, UP
         * @param [Object* parent, Object *child]
         */
        static const gc::StringName MESSAGE_REMOVE_CHILD;

        /**
         * Notifity when the material changed, UP
         * @param [Object *child, Material *old_material]
         */
        static const gc::StringName MESSAGE_CHANGE_MATERIAL;

        /**
         * Notifity when the pose updated, DOWN
         * @param [Object *child]
         */
        static const gc::StringName MESSAGE_UPDATE_POSE;

        /**
         * Notifity when receive the touch event
         * @param [EventType, Vector2f screenPoint]
         */
        static const gc::StringName MESSAGE_TOUCH_EVENT;

        void sendMessage(const gc::StringName &key, NotifyDirection direction, const gc::RArray *vars = NULL);
        METHOD _FORCE_INLINE_ void sendMessageV(const gc::StringName &key, NotifyDirection direction, const gc::RArray &vars) {
            sendMessage(key, direction, &vars);
        }

        /**
         * Material
         */
        METHOD _FORCE_INLINE_ virtual const gc::Ref<Material> &getMaterial() const {return material;}
        METHOD virtual void setMaterial(const gc::Ref<Material> &material) {
            gc::Ref<Material> old = this->material;
            this->material = material;
            _changeMaterial(this, *old);
        }

        /**
         * Mesh
         */
        METHOD _FORCE_INLINE_ virtual const gc::Ref<Mesh> &getMesh() const {return mesh;}
        METHOD _FORCE_INLINE_ virtual void setMesh(const gc::Ref<Mesh> &mesh) {
            this->mesh = mesh;
        }
    
        /**
         * Pose
         */
        METHOD _FORCE_INLINE_ const Matrix4 &getPose() const {return pose;}
        METHOD void setPose(const Matrix4 &pose);
        PROPERTY(pose, getPose, setPose)
    
        void addPoseCallback(void *target, gc::ActionCallback callback, void *data);
        void *removePoseCallback(void *target);

        METHOD virtual void rotate(float radias, const Vector3f &axis);
        METHOD virtual void translate(const Vector3f &trans);
        METHOD virtual void scale(const Vector3f &scale);
        _FORCE_INLINE_ const Vector3f &getPosition() const {
            return position;
        }
        void setPosition(const Vector3f &pos);
        _FORCE_INLINE_ const Quaternion &getRotation() const {
            return rotation;
        }
        void setRotation(const Quaternion & rot);
        _FORCE_INLINE_ const Vector3f &getScale() const {
            return _scale;
        }
        void setScale(const Vector3f &scale);
        METHOD virtual const Matrix4 &getGlobalPose() const;
    
        /**
         * Name of object.
         */
        METHOD _FORCE_INLINE_ const gc::StringName &getName() {
            return name;
        }
        METHOD _FORCE_INLINE_ void setName(const gc::StringName &name) {
            this->name = name;
        }

        /**
         * Parent
         */
        METHOD _FORCE_INLINE_ Object3D *getParent() const {return parent;}

        METHOD _FORCE_INLINE_ Mask getMask() const {return mask;}
        METHOD void setMask(Mask mask);

        METHOD _FORCE_INLINE_ Mask getHitMask() const {return hitMask;}
        METHOD void setHitMask(Mask mask);

        /**
         * Physics
         */

        METHOD void setCollision(bool collision);
        METHOD _FORCE_INLINE_ bool getCollition() const { return collision;}

        METHOD const gc::Ref<Mesh> &getCollider() const;
        METHOD _FORCE_INLINE_ void setCollider(gc::Ref<Mesh> collider) { this->collider = collider; }

        /**
         * Children manage
         */
        METHOD virtual void add(const gc::Ref<Object3D> &object);
        METHOD virtual void remove(const gc::Ref<Object3D> &object);
        _FORCE_INLINE_ const std::list<gc::Ref<Object3D> > &getChildren() const {return children;}

        _FORCE_INLINE_ void setOnEvent(gc::ActionCallback event, void *data) {
            on_event.callback = event;
            on_event.data = data;
        }

        METHOD void setEnable(bool enable);
        METHOD _FORCE_INLINE_ bool getEnable() {return enable;}

        METHOD bool isFinalEnable();

        virtual void copyParameters(const gc::Ref<Object3D> &other);

        /**
         * Traversal
         */
        typedef bool(TraversalChecker)(Object3D *target, void *data);
        typedef void(TraversalHandle)(Object3D *target, void *data);
        void traversal(TraversalChecker checker, TraversalHandle dofun, void *data);

        void change();

        /**
         * 标记这个对象是否是一个，独立对象。
         * 独立对象在渲染引擎中不会自动优化为一个。
         */
        _FORCE_INLINE_ void setSingle(bool single) { this->single = single;}
        _FORCE_INLINE_ bool isSingle() { return single;}

        _FORCE_INLINE_ Renderer *getRenderer() {
            return renderer;
        }

        Object3D();
        ~Object3D();

        /**
         * Set update
         */
        void setUpdateEnable(bool enable);
        _FORCE_INLINE_ bool getUpdateEnable() {return update_enable;}

        void setStepEnable(bool enable);
        _FORCE_INLINE_ bool getStepEnable() {return step_enable;}
    
        _FORCE_INLINE_ void setStatic(bool s) {_static = s;}
        _FORCE_INLINE_ bool isStatic() {return _static;}

        INITIALIZE(Object3D, PARAMS(gc::Ref<Material> &material, gc::Ref<Mesh> &mesh),
                   this->material = material;
                           this->mesh = mesh;
                           parent = NULL;
        )

    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_PROPERTY(cls, "pose", ADD_METHOD(cls, Object, getPose), ADD_METHOD(cls, Object, setPose));
            ADD_METHOD(cls, Object, sendMessageV);
            ADD_METHOD(cls, Object, getMaterial);
            ADD_METHOD(cls, Object, setMaterial);
            ADD_METHOD(cls, Object, getMesh);
            ADD_METHOD(cls, Object, setMesh);
            ADD_METHOD(cls, Object, rotate);
            ADD_METHOD(cls, Object, translate);
            ADD_METHOD(cls, Object, scale);
            ADD_METHOD(cls, Object, getGlobalPose);
            ADD_METHOD(cls, Object, getName);
            ADD_METHOD(cls, Object, setName);
            ADD_METHOD(cls, Object, getParent);
            ADD_METHOD(cls, Object, getMask);
            ADD_METHOD(cls, Object, setMask);
            ADD_METHOD(cls, Object, getHitMask);
            ADD_METHOD(cls, Object, setHitMask);
            ADD_METHOD(cls, Object, setCollision);
            ADD_METHOD(cls, Object, getCollition);
            ADD_METHOD(cls, Object, getCollider);
            ADD_METHOD(cls, Object, setCollider);
            ADD_METHOD(cls, Object, add);
            ADD_METHOD(cls, Object, remove);
            ADD_METHOD(cls, Object, setEnable);
            ADD_METHOD(cls, Object, getEnable);
            ADD_METHOD(cls, Object, isFinalEnable);
        ON_LOADED_END
    CLASS_END
}


#endif //HIRENDER_OBJECT_H
