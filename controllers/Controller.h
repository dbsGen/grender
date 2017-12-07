//
// Created by gen on 16/7/10.
//

#ifndef VOIPPROJECT_CONTROLLER_H
#define VOIPPROJECT_CONTROLLER_H

#include <object/Object.h>
#include <object/ui/View.h>
#include <object/Camera.h>
#include <render_define.h>

using namespace hicore;

namespace hirender {
    class Renderer;
    class NavigationController;

    CLASS_BEGIN_N(Controller, RefObject)

    private:
        Renderer *renderer;
        Ref<Object> main_object;
        Ref<Camera> solid_camera;
        Ref<Camera> ui_camera;
        Ref<View>   icon;
        Controller *parent;

        void setCameraSize(const HSize &size);

        friend class Renderer;

    protected:

        virtual void onLoad(const Ref<Object> &object);
        virtual void onUnload(const Ref<Object> &object);
        EVENT(void, _onLoad, const Ref<Object> &object)
        EVENT(void, _onUnload, const Ref<Object> &object)

        /**
         * Called when attach or disattach to the renderer
         */
        _FORCE_INLINE_ virtual void onAttach(Renderer *renderer) {}
        _FORCE_INLINE_ virtual void onDisattach(Renderer *renderer) {}
        EVENT(void, _onAttach)
        EVENT(void, _onDisattach)
    
        _FORCE_INLINE_ virtual long _appearDuring() {return 0;}
        _FORCE_INLINE_ virtual long _disappearDuring() {return 0;}
        EVENT(long, _appearDuring)
        EVENT(long, _disappearDuring)
    
        _FORCE_INLINE_ virtual void _appearProcess(float percent) {}
        _FORCE_INLINE_ virtual void _disappearProcess(float percent) {}
        EVENT(void, _appearProcess, float percent)
        EVENT(void, _disappearProcess, float percent)
    
        virtual void onResize(const HSize &size);

    public:
        void enableSolidCamera();
        _FORCE_INLINE_ const Ref<Camera> &getSolidCamera() {
            return solid_camera;
        }
        void enableUICamera();
        _FORCE_INLINE_ const Ref<Camera> &getUICamera() {
            return ui_camera;
        }

        virtual void load();
        virtual void unload();

        virtual void willAppear();
        virtual void willDisappear();
        virtual void afterAppear();
        virtual void afterDisappear();
        EVENT(void, _willAppear)
        EVENT(void, _willDisappear)
        EVENT(void, _afterAppear)
        EVENT(void, _afterDisappear)

        /**
         * Return the time(millionseconds) of appear(disappear) animation if no animation return 0.
         */
        long appearDuring();
        long disappearDuring();
    
        METHOD _FORCE_INLINE_ void setParent(const Ref<Controller> &parent) {this->parent = *parent;}
        /**
         * The process of appear called during animation
         * @param percent from 0 to 1
         */
        void appearProcess(float percent);
        void disappearProcess(float percent);
    
        /**
         * Attach to renderer
         */
        virtual void attach(Renderer *renderer);
        virtual void disattach(Renderer *renderer);

        METHOD const Ref<Object> &getMainObject();
        METHOD _FORCE_INLINE_ Renderer *getRenderer() {return renderer;}
        METHOD _FORCE_INLINE_ Ref<Controller> getParent() {return parent;}
        PROPERTY(parent, getParent, setParent)
    
        _FORCE_INLINE_ void setIcon(const Ref<View> &icon) {
            this->icon = icon;
        }
        METHOD _FORCE_INLINE_ const Ref<View> &getIcon() {
            return icon;
        }

        METHOD Reference getNav();
        _FORCE_INLINE_ bool isLoaded() {
            return main_object != nullptr;
        }

        _FORCE_INLINE_ Controller() : parent(NULL), renderer(NULL) {}
        ~Controller();

    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_PROPERTY(cls, "parent", ADD_METHOD(cls, Controller, getParent), ADD_METHOD(cls, Controller, setParent));
            ADD_METHOD(cls, Controller, getMainObject);
            ADD_METHOD(cls, Controller, getRenderer);
            ADD_METHOD(cls, Controller, getNav);
            ADD_METHOD(cls, Controller, getIcon);
            ADD_METHOD(cls, Controller, setIcon);
        ON_LOADED_END
    CLASS_END
}


#endif //VOIPPROJECT_CONTROLLER_H
