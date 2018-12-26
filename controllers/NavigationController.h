//
// Created by gen on 16/7/17.
//

#ifndef VOIPPROJECT_NAVIGATIONCONTROLLER_H
#define VOIPPROJECT_NAVIGATIONCONTROLLER_H


#include "Controller.h"
#include "../object/ui/NavControl.h"
#include <render_define.h>

namespace gr {
    class Tween;
    
    CLASS_BEGIN_N(NavigationControllerItem, gc::RefObject)
    
    std::vector<gc::Ref<Controller> > controllers;
    int default_index;
    int index;
    
    const gc::Ref<Controller> &getCurrentController();
    
    friend class NavigationController;
    
public:
    NavigationControllerItem() : default_index(0), index(-1) {}
    NavigationControllerItem(const gc::Ref<Controller> &control) : NavigationControllerItem() {
        controllers.push_back(control);
    }
    ~NavigationControllerItem();
    
    void push(const gc::Ref<Controller> &control) {
        controllers.push_back(control);
    }
    int getDefaultIndex() {
        return default_index;
    }
    void setDefaultIndex(int index) {
        default_index = index;
    }
    
    CLASS_END

    CLASS_BEGIN_N(NavigationController, Controller)
    
        gc::Ref<NavControl> nav_control;
        std::vector<gc::Ref<NavigationControllerItem> > items;

        static void tweenAppearUpdate(void *tween, void *percent, void *data);
        static void tweenAppearComplete(void *tween, void *n, void *data);
        static void tweenDisappearUpdate(void *tween, void *percent, void *data);
        static void tweenDisappearComplete(void *tween, void *n, void *data);
    
        void changeController(const gc::Ref<Controller> &current_controller,
                              const gc::Ref<Controller> &controller,
                              bool animate = false);
        static void onControlClicked(void *sender, void *send_data, void *data);
        gc::Ref<NavControlItem> makeNavItem();

    protected:
        virtual void onLoad(const gc::Ref<Object3D> &object);
        virtual void onUnload(const gc::Ref<Object3D> &object);
        virtual void onAttach(Renderer *renderer);
        virtual void onDisattach(Renderer *renderer);
    
    public:
        virtual void load();
        virtual void unload();

        virtual void attach(Renderer *renderer);
        virtual void disattach(Renderer *renderer);
        METHOD const gc::Ref<Controller> &getCurrentController();

        METHOD void push(const gc::Ref<NavigationControllerItem> &item, bool animate = false);
        METHOD gc::Ref<NavigationControllerItem> pop(bool animate = false);

        _FORCE_INLINE_ NavigationController() {}
        INITIALIZE(NavigationController, const gc::Ref<NavigationControllerItem> &item,
                   items.push_back(item);
        )

    protected:
        ON_LOADED_BEGIN(cls, Controller)
            ADD_METHOD(cls, NavigationController, getCurrentController);
            ADD_METHOD(cls, NavigationController, push);
            ADD_METHOD(cls, NavigationController, pop);
        ON_LOADED_END
    CLASS_END

}


#endif //VOIPPROJECT_NAVIGATIONCONTROLLER_H
