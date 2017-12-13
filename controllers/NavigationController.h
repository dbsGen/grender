//
// Created by gen on 16/7/17.
//

#ifndef VOIPPROJECT_NAVIGATIONCONTROLLER_H
#define VOIPPROJECT_NAVIGATIONCONTROLLER_H


#include "Controller.h"
#include "../object/ui/NavControl.h"
#include <render_define.h>

using namespace gcore;

namespace gr {
    class Tween;
    
    CLASS_BEGIN_N(NavigationControllerItem, RefObject)
    
    vector<Ref<Controller> > controllers;
    int default_index;
    int index;
    
    const Ref<Controller> &getCurrentController();
    
    friend class NavigationController;
    
public:
    NavigationControllerItem() : default_index(0), index(-1) {}
    NavigationControllerItem(const Ref<Controller> &control) : NavigationControllerItem() {
        controllers.push_back(control);
    }
    ~NavigationControllerItem();
    
    void push(const Ref<Controller> &control) {
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
    
        Ref<NavControl> nav_control;
        vector<Ref<NavigationControllerItem> > items;

        static void tweenAppearUpdate(void *tween, void *percent, void *data);
        static void tweenAppearComplete(void *tween, void *n, void *data);
        static void tweenDisappearUpdate(void *tween, void *percent, void *data);
        static void tweenDisappearComplete(void *tween, void *n, void *data);
    
        void changeController(const Ref<Controller> &current_controller, const Ref<Controller> &controller, bool animate = false);
        static void onControlClicked(void *sender, void *send_data, void *data);
        Ref<NavControlItem> makeNavItem();

    protected:
        virtual void onLoad(const Ref<Object> &object);
        virtual void onUnload(const Ref<Object> &object);
        virtual void onAttach(Renderer *renderer);
        virtual void onDisattach(Renderer *renderer);
    
    public:
        virtual void load();
        virtual void unload();

        virtual void attach(Renderer *renderer);
        virtual void disattach(Renderer *renderer);
        METHOD const Ref<Controller> &getCurrentController();

        METHOD void push(const Ref<NavigationControllerItem> &item, bool animate = false);
        METHOD Ref<NavigationControllerItem> pop(bool animate = false);

        _FORCE_INLINE_ NavigationController() {}
        INITIALIZE(NavigationController, const Ref<NavigationControllerItem> &item,
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
