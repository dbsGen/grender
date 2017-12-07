//
// Created by gen on 16/7/10.
//

#include "Controller.h"
#include "NavigationController.h"
#include "../Renderer.h"

using namespace hirender;

Controller::~Controller() {
}

const Ref<Object> &Controller::getMainObject() {
    if (!main_object) load();
    return main_object;
}

void Controller::enableSolidCamera() {
    if (!solid_camera) {
        solid_camera = new Camera;
        solid_camera->setName("Camera");
        solid_camera->setHitMask(MaskMask ^ UIMask);
        solid_camera->setType(Camera::Perspective);
        solid_camera->setClean(true);
        if (renderer) {
            const HSize &size = renderer->getSize();
            float as = size.x() / size.y();
            solid_camera->setRect(HRect(-as, -1, as*2, 2));
        }
        if (main_object) {
            main_object->add(solid_camera);
        }
    }
}

void Controller::enableUICamera() {
    if (!ui_camera) {
        ui_camera = new Camera;
        ui_camera->setName("UI Camera");
        ui_camera->setHitMask(UIMask);
        if (renderer) {
            const HSize &size = renderer->getSize();
            ui_camera->setRect(HRect(0, size.y(), size.x(), -size.y()));
        }
        ui_camera->setNear(1);
        ui_camera->setFar(5000);
        ui_camera->setDepth(1);
        if (main_object) {
            main_object->add(ui_camera);
        }
    }
}

void Controller::onResize(const HSize &size) {
    setCameraSize(size);
}

void Controller::setCameraSize(const HSize &size) {
    float as = size.x() / size.y();
    if (solid_camera)
        solid_camera->setRect(HRect(-as, -1, as*2, 2));
    if (ui_camera)
        ui_camera->setRect(HRect(0, size.y(), size.x(), -size.y()));
}

void Controller::load() {
    main_object = new Object;
    if (solid_camera) {
        main_object->add(solid_camera);
    }
    if (ui_camera) {
        main_object->add(ui_camera);
    }
    onLoad(main_object);
    Variant *vs[1];
    Variant v1(main_object);
    vs[0] = &v1;
    apply("_onLoad", NULL, (const Variant **)vs, 1);
}

void Controller::unload() {
    if (main_object) {
        onUnload(main_object);
        Variant *vs[1];
        Variant v1(main_object);
        vs[0] = &v1;
        apply("_onUnload", NULL,(const Variant **)vs, 1);
        main_object = NULL;
    }
}

void Controller::onLoad(const Ref<Object> &object) {
}

void Controller::onUnload(const Ref<Object> &object) {
}

Reference Controller::getNav() {
    if (getParent()) {
        return getParent();
    }
    return Reference::null();
}

long Controller::appearDuring() {
    Variant r;
    apply("_appearDuring", &r);
    if (r.getType()) {
        return r;
    }else {
        return _appearDuring();
    }
}
long Controller::disappearDuring() {
    Variant r;
    apply("_disappearDuring", &r);
    if (r.getType()) {
        return r;
    }else {
        return _disappearDuring();
    }
}

void Controller::appearProcess(float percent) {
    Variant *vs[1];
    Variant v1(percent);
    vs[0] = &v1;
    apply("_appearProcess", NULL, (const Variant **)vs, 1);
    _appearProcess(percent);
}

void Controller::disappearProcess(float percent) {
    Variant *vs[1];
    Variant v1(percent);
    vs[0] = &v1;
    apply("_disappearProcess", NULL, (const Variant **)vs, 1);
    _disappearProcess(percent);
}

void Controller::willAppear() {
    apply("_willAppear");
}

void Controller::willDisappear() {
    apply("_willDisappear");
}

void Controller::afterAppear() {
    apply("_afterAppear");
}

void Controller::afterDisappear() {
    apply("_afterDisappear");
}

void Controller::attach(Renderer *renderer) {
    this->renderer = renderer;
    setCameraSize(renderer->getSize());
    onAttach(renderer);
    apply("_onAttach");
}

void Controller::disattach(Renderer *renderer) {
    onDisattach(renderer);
    apply("_onDisattach");
    if (this->renderer == renderer)
        this->renderer = NULL;
}
