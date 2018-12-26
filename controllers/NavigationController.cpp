//
// Created by gen on 16/7/17.
//

#include <utils/tween/Tween.h>
#include "NavigationController.h"
#include <math/Math.hpp>
#include <Renderer.h>
#include <utils/input/TouchInput.h>


using namespace gr;
using namespace gc;

const Ref<Controller> &NavigationControllerItem::getCurrentController() {
    if (index == -1) {
        index = clamp(default_index, 0, (int)controllers.size() - 1);
    }
    return controllers[index];
}

NavigationControllerItem::~NavigationControllerItem() {
    for (auto it = controllers.begin(), _e = controllers.end(); it != _e; ++it) {
        (*it)->unload();
    }
}

void NavigationController::changeController(const Ref<Controller> &current_controller, const Ref<Controller> &controller, bool animate) {
    controller->setParent(this);
    getMainObject()->add(controller->getMainObject());
    controller->willAppear();
    if (getRenderer()) controller->attach(getRenderer());
    if (animate) {
        long during = controller->appearDuring();
        if (during > 0) {
            Tween::cancel(controller);
            const Ref<Tween> &tween = Tween::New(controller, during);
            tween->add(tweenAppearUpdate, *controller);
            tween->setOnComplete(tweenAppearComplete, *controller);
            tween->start();
        }
        if (current_controller) {
            const Ref<Controller> &current = current_controller;
            current->willDisappear();
            Tween::cancel(current);
            during = current->disappearDuring();
            if (during > 0) {
                const Ref<Tween> &tween = Tween::New(current, during);
                tween->add(tweenDisappearUpdate, *controller);
                tween->setOnComplete(tweenDisappearComplete, *controller);
                tween->start();
            }else {
                getMainObject()->remove(current->getMainObject());
            }
        }
    }else {
        if (current_controller) {
            const Ref<Controller> &current = current_controller;
            current->willDisappear();
            getMainObject()->remove(getCurrentController()->getMainObject());
            current->afterDisappear();
        }
        controller->afterAppear();
    }

}

void NavigationController::onControlClicked(void *sender, void *send_data, void *data) {
    NavigationController *ctrl = (NavigationController*)data;
    Ref<NavigationControllerItem> &item = ctrl->items.back();
    const Ref<Controller> &cc = ctrl->getCurrentController();
    NavControlItem::NavButtonIndex idx = *(NavControlItem::NavButtonIndex*)send_data;
    if (idx >= 0 && item->index != idx) {
        item->index = idx;
        ctrl->changeController(cc, item->getCurrentController(), true);
    }else if (idx == -1) {
        ctrl->pop(true);
    }
}

Ref<NavControlItem> NavigationController::makeNavItem() {
    NavControlItem *item = new NavControlItem;
    const Ref<NavigationControllerItem> &b = items.back();
    for (auto it = b->controllers.begin(), _e = b->controllers.end(); it != _e; ++it) {
        item->pushSubIcon((*it)->getIcon());
    }
    item->pushOnClicked(&NavigationController::onControlClicked, this);
    item->setDefaultIndex(b->default_index);
    return item;
}

void NavigationController::push(const Ref<NavigationControllerItem> &item, bool animate) {
    if (getRenderer()){
        for (auto it = item->controllers.begin(), _e = item->controllers.end(); it != _e; ++it) {
            (*it)->attach(getRenderer());
        }
    }
    if (isLoaded()) {
        for (auto it = item->controllers.begin(), _e = item->controllers.end(); it != _e; ++it) {
            (*it)->setParent(this);
        }
        changeController(getCurrentController(), item->getCurrentController(), animate);
    }
    items.push_back(item);
    if (isLoaded()) {
        nav_control->pushItem(makeNavItem(), animate);
    }
}

Ref<NavigationControllerItem> NavigationController::pop(bool animate) {
    if (items.size() > 1) {
        Ref<NavigationControllerItem> last = items.back();
        for (auto it = last->controllers.begin(), _e = last->controllers.end(); it != _e; ++it) {
            (*it)->disattach(getRenderer());
            (*it)->setParent(NULL);
        }
        items.pop_back();
        changeController(last->getCurrentController(), getCurrentController(), animate);
        nav_control->popItem(animate);
        return last;
    }else {
        return Ref<NavigationControllerItem>::null();
    }
}

const Ref<Controller> &NavigationController::getCurrentController() {
    if (items.size() > 0) {
        const Ref<NavigationControllerItem> &item = items.back();
        return item->getCurrentController();
    }
    return Ref<Controller>::null();
}

void NavigationController::onLoad(const Ref<Object3D> &object) {
    const Ref<NavigationControllerItem> &item = items.back();
    for (auto it = item->controllers.begin(), _e = item->controllers.end(); it != _e; ++it) {
        (*it)->setParent(this);
    }
    const Ref<Controller> &ctrl = getCurrentController();
    nav_control = new NavControl;
    if (ctrl) {
        object->add(ctrl->getMainObject());
        nav_control->pushItem(makeNavItem(), false);
    }
    object->add(nav_control);
}

void NavigationController::onUnload(const Ref<Object3D> &object) {
    nav_control = nullptr;
}

void NavigationController::onAttach(Renderer *renderer) {
    enableUICamera();
    getUICamera()->setCleanColor(Color(1,1,1,1));
    const Ref<Plugin> &tp = renderer->plugin(TouchInput::getClass()->getFullname());
    TouchInput *touch = (*tp)->cast_to<TouchInput>();
    touch->addCamera(getUICamera());
}

void NavigationController::onDisattach(Renderer *renderer) {
    const Ref<Plugin> &tp = renderer->plugin(TouchInput::getClass()->getFullname());
    TouchInput *touch = (*tp)->cast_to<TouchInput>();
    touch->clear();
}

void NavigationController::load() {
    Controller::load();
    const Ref<Controller> &ctrl = getCurrentController();
    if (ctrl && !ctrl->isLoaded()) ctrl->load();
}

void NavigationController::unload() {
    const Ref<Controller> &ctrl = getCurrentController();
    if (ctrl && ctrl->isLoaded()) ctrl->unload();
    Controller::unload();
}

void NavigationController::attach(Renderer *renderer) {
    Controller::attach(renderer);
    for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
        const Ref<NavigationControllerItem> &item = *it;
        for (auto _it = item->controllers.begin(), __e = item->controllers.end(); _it != __e; ++_it) {
            (*_it)->attach(renderer);
        }
    }
}

void NavigationController::disattach(Renderer *renderer) {
    for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
        const Ref<NavigationControllerItem> &item = *it;
        for (auto _it = item->controllers.begin(), __e = item->controllers.end(); _it != __e; ++_it) {
            (*_it)->disattach(renderer);
        }
    }
    Controller::disattach(renderer);
}

void NavigationController::tweenAppearUpdate(void *tween, void *percent, void *data) {
    Controller *c = (Controller *)data;
    c->appearProcess(*(double*) percent);
}

void NavigationController::tweenAppearComplete(void *tween, void *n, void *data) {
    Controller *c = (Controller *)data;
    c->afterAppear();
}

void NavigationController::tweenDisappearUpdate(void *tween, void *percent, void *data) {
    Controller *c = (Controller *)data;
    c->disappearProcess(*(double*) percent);
}

void NavigationController::tweenDisappearComplete(void *tween, void *n, void *data) {
    Controller *c = (Controller*)data;
    Controller *p = *c->getParent();
    if (p) {
        p->getMainObject()->remove(c->getMainObject());
    }
    c->afterDisappear();
}
