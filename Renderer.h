//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_RENDERER_H
#define HIRENDER_RENDERER_H

#include <list>
#include <mutex>
#include <thread>

#include <render_define.h>
#include <object/Camera.h>
#include <controllers/Controller.h>
#include <utils/tween/Tween.h>
#include <physics/Ray.h>
#include "utils/LoopThread.h"
#include "core/Define.h"
#include "object/Object.h"
#include "Plugin.h"
#include "types.h"
#include "core/Callback.h"
#include "render_define.h"

using namespace std;

namespace gr {
    class RendererIMP;

    CLASS_BEGIN_N(Renderer, RefObject)
    private:
        static Renderer *shared_instance;

        CLASS_BEGIN_N(RenderRoot, Object)
        private:
            bool need_update;
            _FORCE_INLINE_ void updated() {need_update=false;}
            void changed();

            friend class Renderer;

            static bool travCheck(Object *, void *data);
            static void travAddDo(Object *, void *data);
            static void travRemoveDo(Object *, void *data);

        protected:
            virtual bool onMessage(const StringName &key, const Array *vars);

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

        Ref<Controller> main_controller;

        LoopThread fixed_thread;
        mutex render_mtx;
        Time last_frame;
        Time last_step_time;
        static float screen_scale;

        HSize   size;

        _FORCE_INLINE_ void addObject(Object *object) {
            object->_added(this);
        }
        _FORCE_INLINE_ void removeObject(Object *object) {
            object->_removed(this);
        }

        pointer_map plugins;
        mutex will_do_mtx;
        pointer_list will_do;
        vector<RefCallback >  will_callback;
        vector<RefCallback >  next_frame_callback;
    
        int required_reset_count;
        bool required_rendering;
        bool rendering;
        bool need_render;

        bool dynamic_frame;

        int fixed_rate;
        Time fixed_interval;

        int _draw_call_count;
        int _frame_per_second;

        int draw_call_count;
        int frame_per_second;

        Time last_count_time;
        static void fixedThreadStart(void *loop_thread, void *_, void *that);
        static void fixedThreadOver(void *loop_thread, void *_, void *that);
        static void fixedProcess(void *loop_thread, void *_, void *that);
        void step();

        void callRender();
        void setCurrentThread(bool sub);

        static void _destroy(Renderer *renderer);

        friend class RenderRoot;
        friend class RendererIMP;
    public:
        enum ThreadState {
            SubThread,
            MainThread,
        };
    
        static const StringName NOTIFICATION_PREV_FRAME;
        static const StringName NOTIFICATION_POST_FRAME;

        /**
         * Notified when render frame
         * @params [Time delta]
         */
        static const StringName NOTIFICATION_FRAME_STEP;
        static const StringName NOTIFICATION_FIXED_UPDATE;
        static const StringName NOTIFICATION_RENDERER_DELETE;

        /**
         * Notified event not handled
         * @params [EventType type, Point screenPoint]
         */
        static const StringName NOTIFICATION_HANDLE_EVENT;

        METHOD static  Renderer *sharedInstance();
    
        static ThreadState currentThread();

        Renderer();
        ~Renderer();

        METHOD void requireRender();
        _FORCE_INLINE_ virtual void _requireRender() {}
        EVENT(void, _requireRender);
        _FORCE_INLINE_ virtual void _needUpdate() {}
        METHOD void render();
        /**
         * Fixed update.
         */
        EVENT(void, _step);

        METHOD const Ref<Camera> &getCamera(int index);

        _FORCE_INLINE_ METHOD const HSize &getSize() {
            return size;
        }

        METHOD void setSize(const HSize &size);

        // Objects manager

        METHOD void add(const Ref<Object> &object);
        METHOD void remove(const Ref<Object> &object);
        _FORCE_INLINE_ const list<Ref<Object>> &getObjects() {return root.getChildren();}

        METHOD void setMainController(const Ref<Controller> &controller);
        METHOD const Ref<Controller> &getMainController();
        PROPERTY(main_controller, getMainController, setMainController)

        METHOD void attach(const Ref<Plugin> &plugin);
        METHOD const Ref<Plugin> &plugin(const StringName &name);
        METHOD void disattach(const StringName &name);

        METHOD static Time time();

        _FORCE_INLINE_ bool isDynamicFrame() { return dynamic_frame; }
        _FORCE_INLINE_ void setDynamicFrame(bool dynamic_frame) { this->dynamic_frame = dynamic_frame; }

        _FORCE_INLINE_ void setFixedRate(int fixed_rate) {
            this->fixed_rate = fixed_rate;
            fixed_interval = 1.0/fixed_rate;
        }
        _FORCE_INLINE_ int getFixedRate() {
            return fixed_rate;
        }
        _FORCE_INLINE_ Time getFixedInterval() {
            return fixed_interval;
        }
        _FORCE_INLINE_ Time getLastFrameTime() {
            return last_frame;
        }
        _FORCE_INLINE_ RendererIMP *getIMP() { return imp; }
    
        void *doOnMainThread(void *target, ActionCallback callback, void *data = nullptr);
        void *cancelDoOnMainThread(void *target);
        void doNextFrame(const RefCallback &callback) {
            next_frame_callback.push_back(callback);
        }
    
        void doOnMainThread(const RefCallback &callback) {
            will_callback.push_back(callback);
        }

        _FORCE_INLINE_ int getDrawCallCount() {
            return draw_call_count;
        }
        _FORCE_INLINE_ int getFramePerSecond() {
            return frame_per_second;
        }

        void reload(Object *object);
        void maskChange(Object *object, Mask from, Mask to);
        void hitMaskChange(Object *object, Mask from, Mask to);

        static void destroy(Renderer *renderer);
    
        _FORCE_INLINE_ static float getScreenScale() {
            return screen_scale;
        }
        _FORCE_INLINE_ static void setScreenScale(float screen_scale) {
            Renderer::screen_scale = screen_scale;
        }

    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_PROPERTY(cls, "main_controller", ADD_METHOD(cls, Renderer, getMainController), ADD_METHOD(cls, Renderer, setMainController));
            ADD_METHOD(cls, Renderer, sharedInstance);
            ADD_METHOD(cls, Renderer, requireRender);
            ADD_METHOD(cls, Renderer, render);
            ADD_METHOD(cls, Renderer, getSize);
            ADD_METHOD(cls, Renderer, setSize);
            ADD_METHOD(cls, Renderer, add);
            ADD_METHOD(cls, Renderer, remove);
            ADD_METHOD(cls, Renderer, attach);
            ADD_METHOD(cls, Renderer, disattach);
            ADD_METHOD(cls, Renderer, time);
        ON_LOADED_END
    CLASS_END


    CLASS_BEGIN_TNV(RendererIMP, Imp, 1, Renderer)

    protected:

        _FORCE_INLINE_ void addObject(Object *object) {
            getTarget()->addObject(object);
        }
        _FORCE_INLINE_ void removeObject(Object *object) {
            getTarget()->removeObject(object);
        }
//        _FORCE_INLINE_ void drawBegin() {
//        }
        _FORCE_INLINE_ void drawCalled() {
            ++this->getTarget()->_draw_call_count;
        }
        _FORCE_INLINE_ void drawEnd() {
            this->getTarget()->draw_call_count = this->getTarget()->_draw_call_count;
            this->getTarget()->_draw_call_count = 0;
        }

        virtual void prepare() = 0;
        virtual void add(const Ref<Object> &object) = 0;
        virtual void remove(const Ref<Object> &object) = 0;
        virtual void reload(Object *object, Material *old_mat) = 0;
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
