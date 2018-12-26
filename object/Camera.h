//
// Created by gen on 16/6/22.
//

#ifndef HI_RENDER_PROJECT_ANDROID_CAMERA_H
#define HI_RENDER_PROJECT_ANDROID_CAMERA_H

#include "Object3D.h"
#include <physics/Ray.h>
#include "../render_define.h"

namespace gr {
    CLASS_BEGIN_N(Camera, Object3D)

    public:
        enum Type {
            Orthographic,
            Perspective
        };

    private:
        Matrix4     projection;
        bool        dirty_projection;
        Type        type;
        Rect        rect;
        Rect        viewport;
        float       near,
                    far;
        float       fov;
        int         depth;

        gc::Ref<Object3D> current_touch_object;
        bool        focus;
    
        bool        clean;
        Color       clean_color;
    
        gc::Reference   output_target;

    protected:
        virtual bool onMessage(const gc::StringName &key, const gc::RArray *vars);

    public:
        const Matrix4 &getProjection();
        _FORCE_INLINE_ void setProjection(const Matrix4 &projection) {
            this->projection = projection;
            dirty_projection = false;
        }
        _FORCE_INLINE_ Type getType() const {
            return type;
        }
        _FORCE_INLINE_ void setType(Type type) {
            if (this->type != type) {
                this->type = type;
                dirty_projection = true;
            }
        }
        _FORCE_INLINE_ const Rect &getRect() const {
            return rect;
        }
        _FORCE_INLINE_ void setRect(const Rect &rect) {
            if (this->rect != rect) {
                this->rect = rect;
                dirty_projection = true;
            }
        }
        _FORCE_INLINE_ const Rect &getViewport() const {
            return viewport;
        }
        _FORCE_INLINE_ void setViewport(const Rect &vp) {
            if (viewport != vp) {
                viewport = vp;
                dirty_projection = true;
            }
        }
        _FORCE_INLINE_ float getNear() const {
            return near;
        }
        _FORCE_INLINE_ void setNear(float near) {
            if (this->near != near) {
                this->near = near;
                dirty_projection = true;
            }
        }
        _FORCE_INLINE_ float getFar() const {
            return far;
        }
        _FORCE_INLINE_ void setFar(float far) {
            if (this->far != far) {
                this->far = far;
                dirty_projection = true;
            }
        }
        _FORCE_INLINE_ float getFov() const {
            return fov;
        }
        _FORCE_INLINE_ void setFov(float fov) {
            if (this->fov != fov) {
                this->fov = fov;
                dirty_projection = true;
            }
        }
        _FORCE_INLINE_ int getDepth() const {
            return depth;
        }
        _FORCE_INLINE_ void setDepth(int depth) {
            this->depth = depth;
        }

        Point screenToWorld(const Point &screen_point);
        Point worldToScreen(const Point &screen_point);

        virtual bool sortCompare(Object3D *o1, Object3D *o2);
        virtual void copyParameters(const gc::Ref<Object3D> &other);
        void sendTouchEvent(Object3D::EventType event, const Vector2f &point);
        Ray rayFromScreenPoint(const Vector2f &point);
    
        _FORCE_INLINE_ const gc::Reference &getOuputTarget() {
            return output_target;
        }
    
        _FORCE_INLINE_ void setOutputTarget(const gc::Reference &output) {
            output_target = output;
        }
    
        _FORCE_INLINE_ bool isClean() {
            return clean;
        }
        _FORCE_INLINE_ void setClean(bool clean) {
            this->clean = clean;
        }
    
        const Color &getCleanColor() {
            return clean_color;
        }
    
        void setCleanColor(const Color &color) {
            clean_color = color;
        }

        Camera() : Object3D(),
                   type(Orthographic),
                   near(1), far(5000),
                   projection(Matrix4::identity()),
                   dirty_projection(true),
                   rect(-1, -1, 2, 2),
                   depth(0),
                   viewport(0,0,1,1),
                   focus(false),
                   clean(false),
                   fov(30)
        {
            setHitMask(1);
        }

    CLASS_END
}


#endif //HI_RENDER_PROJECT_ANDROID_CAMERA_H
