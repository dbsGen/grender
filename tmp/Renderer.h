//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_RENDERER_H
#define HIRENDER_RENDERER_H

#include <list>
#include <mutex>

#include <render_define.h>
#include <object/Camera.h>
#include <controllers/Controller.h>
#include <utils/tween/Tween.h>
#include <physics/Ray.h>
#include "core/Define.h"
#include "object/Object.h"
#include "Plugin.h"
#include "render_define.h"

using namespace std;

namespace hirender {
    class RendererIMP;

    CLASS_BEGIN_N(Renderer, RefObject)
    private:
        static Renderer *shared_instance;

        CLASS_BEGIN_N(RenderRoot, Object)
        private:
            bool need_update;
            _FORCE_INLINE_ void updated() {need_update=false;}

            friend class Renderer;

            static bool travCheck(Object *, void *data);
            static void travAddDo(Object *, void *data);
            static void travRemoveDo(Object *, void *data);

        protected:
            virtual bool onMessage(const StringName &key, const Array &vars);
            virtual void changed(Object *object);

        public:
            Renderer *renderer;
            _FORCE_INLINE_ RenderRoot() {
                need_update = false;
            }


        CLASS_END

        friend class Renderer;
        RenderRoot root;

//        static Renderer defaultRenderer;
        mutex mtx;
        RendererIMP *imp;
        vector<Ref<Camera> > cameras;
        Ref<Camera> current_camera;
        Ref<Camera> ui_camera;
        HColor clear_color;

        Ref<Controller> main_controller;

        time_t last_frame;

        HSize   size;

        Material *current_material;

        void process(Object *object, const Matrix4 &projection);
        _FORCE_INLINE_ void addObject(Object *object) {
            object->_added(this);
        }
        _FORCE_INLINE_ void removeObject(Object *object) {
            object->_removed(this);
        }

        ref_map         plugins;
    
        bool rendering;
        bool needRender;

        friend class RenderRoot;
        friend class RendererIMP;
    public:
        static const StringName NOTIFICATION_PREV_FRAME;
        static const StringName NOTIFICATION_POST_FRAME;

        static const StringName NOTIFICATION_FRAME_STEP;
        static const StringName NOTIFICATION_RENDERER_DELETE;

        METHOD static  Renderer *sharedInstance();


        /**
         * Notified event not handled
         * @params [EventType type, Point screenPoint]
         */
        static const StringName NOTIFICATION_HANDLE_EVENT;

        Renderer();
        ~Renderer();

        METHOD void requireRender();
        EVENT(void, _requireRender);
        METHOD void requireRenderNow();
        EVENT(void, _requireRenderNow);
        METHOD void render();

        METHOD const Ref<Camera> &getCurrentCamera();
        METHOD void setCurrentCamera(const Ref<Camera> &camera);
        PROPERTY(current_camera, getCurrentCamera, setCurrentCamera)

        METHOD const Ref<Camera> &getUICamera();

        METHOD const Ref<Camera> &getCamera(int index);

        _FORCE_INLINE_ METHOD const HSize &getSize() {
            return size;
        }

        METHOD void setSize(const HSize &size);

        METHOD void touchBegin(const Vector2f &point);
        METHOD void touchMove(const Vector2f &point);
        METHOD void touchEnd(const Vector2f &point);
        METHOD void touchCancel(const Vector2f &point);

        // Objects manager

        METHOD void add(Ref<Object> object);
        METHOD void remove(Ref<Object> object);
        _FORCE_INLINE_ const list<Ref<Object>> &getObjects() {return root.getChildren();}

        METHOD void setMainController(const Ref<Controller> &controller);
        METHOD const Ref<Controller> &getMainController();
        PROPERTY(main_controller, getMainController, setMainController)

        _FORCE_INLINE_ METHOD const HColor &getClearColor() { return clear_color; }
        _FORCE_INLINE_ METHOD void setClearColor(const HColor &color) { clear_color = color; }
        PROPERTY(clear_color, getClearColor, setClearColor)

        METHOD void attach(const Ref<Plugin> &plugin);
        METHOD void disattach(const string &name);

    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_PROPERTY(cls, "current_camera", ADD_METHOD(cls, Renderer, getCurrentCamera), ADD_METHOD(cls, Renderer, setCurrentCamera));
            ADD_PROPERTY(cls, "main_controller", ADD_METHOD(cls, Renderer, getMainController), ADD_METHOD(cls, Renderer, setMainController));
            ADD_PROPERTY(cls, "clear_color", ADD_METHOD(cls, Renderer, getClearColor), ADD_METHOD(cls, Renderer, setClearColor));
            ADD_METHOD(cls, Renderer, sharedInstance);
            ADD_METHOD(cls, Renderer, requireRender);
            ADD_METHOD(cls, Renderer, requireRenderNow);
            ADD_METHOD(cls, Renderer, render);
            ADD_METHOD(cls, Renderer, getUICamera);
            ADD_METHOD(cls, Renderer, getCamera);
            ADD_METHOD(cls, Renderer, getSize);
            ADD_METHOD(cls, Renderer, setSize);
            ADD_METHOD(cls, Renderer, touchBegin);
            ADD_METHOD(cls, Renderer, touchMove);
            ADD_METHOD(cls, Renderer, touchEnd);
            ADD_METHOD(cls, Renderer, touchCancel);
            ADD_METHOD(cls, Renderer, add);
            ADD_METHOD(cls, Renderer, remove);
            ADD_METHOD(cls, Renderer, attach);
            ADD_METHOD(cls, Renderer, disattach);
        ON_LOADED_END
    CLASS_END


    CLASS_BEGIN_TNV(RendererIMP, Imp, 1, Renderer)

    protected:

        _FORCE_INLINE_ void process(Object *object, const Matrix4 &projection) {
            if (object->isFinalEnable()) getTarget()->process(object, projection);
        }
        _FORCE_INLINE_ void addObject(Object *object) {
            getTarget()->addObject(object);
        }
        _FORCE_INLINE_ void removeObject(Object *object) {
            getTarget()->removeObject(object);
        }

        virtual void add(const Ref<Object> &object) = 0;
        virtual void remove(const Ref<Object> &object) = 0;
        virtual void reload(Object *object) = 0;
        virtual void reload() = 0;
        virtual void render() = 0;
        virtual void updateSize(const HSize &size) = 0;
        virtual void maskChanged(const Ref<Object> &object, Mask from, Mask to) = 0;
        virtual void hitMaskChanged(const Ref<Object> &object, Mask from, Mask to) = 0;

        _FORCE_INLINE_ const vector<Ref<Camera> > &getCameras() {
            return getTarget()->cameras;
        }

        friend class Renderer;

    CLASS_END

}


#endif //HIRENDER_RENDERER_H
