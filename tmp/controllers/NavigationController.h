//
// Created by gen on 16/7/17.
//

#ifndef VOIPPROJECT_NAVIGATIONCONTROLLER_H
#define VOIPPROJECT_NAVIGATIONCONTROLLER_H


#include "Controller.h"
#include <render_define.h>
#include <queue>

using namespace gc;

namespace gr {
    class Tween;

    CLASS_BEGIN_N(NavigationController, Controller)

    private:
        vector<Ref<Controller> > controllers;

        static void tweenAppearUpdate(void *tween, void *percent, void *data);
        static void tweenAppearComplete(void *tween, void *n, void *data);
        static void tweenDisappearUpdate(void *tween, void *percent, void *data);
        static void tweenDisappearComplete(void *tween, void *n, void *data);

    public:
        virtual void load();
        virtual void unload();

        virtual void attach(Renderer *renderer);
        virtual void disattach(Renderer *renderer);
        METHOD _FORCE_INLINE_ Ref<Controller> &getCurrentController() {return controllers.back();}

        METHOD void push(const Ref<Controller> &controller, bool animate = false);
        METHOD Ref<Controller> pop(bool animate = false);

        _FORCE_INLINE_ NavigationController() {}
        INITIALIZE(NavigationController, const Ref<Controller> &controller,
                   push(controller);
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
