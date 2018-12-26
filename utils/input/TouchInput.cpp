//
//  TouchInput.cpp
//  hirender_iOS
//
//  Created by mac on 2016/12/18.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "TouchInput.h"
#include <Renderer.h>

#define EVENT_INCREASE 5

using namespace gr;
using namespace gc;
using namespace std;

TouchInput::TouchInput() {
}

TouchInput::~TouchInput() {
}

TouchInput::Event *TouchInput::begin(int count) {
    mtx.lock();
    EventsItem *ei = new EventsItem(count);
    current_item = ei;
    return ei->events;
}

void TouchInput::commit(TouchInput::Phase phase) {
    if (current_item) {
        EventsItem *ei = (EventsItem*)current_item;
        ei->phase = phase;
        items.push_back(ei);
        current_item = NULL;
    }
    mtx.unlock();
}

void TouchInput::step(Renderer *renderer, Time delta) {
    if (!renderer->isDynamicFrame()) {
        mtx.lock();
        for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
            EventsItem *ei = (TouchInput::EventsItem*)*it;
            process(ei);
            delete ei;
        }
        items.clear();
        mtx.unlock();
    }
}

void TouchInput::fixedStep(Renderer *renderer, Time delta) {
    if (renderer->isDynamicFrame()) {
        mtx.lock();
        for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
            EventsItem *ei = (TouchInput::EventsItem*)*it;
            process(ei);
            delete ei;
        }
        items.clear();
        mtx.unlock();
    }
}

void TouchInput::process(EventsItem *ei) {
    manager(this, ei);
    if (ei->length > 0) {
        list<Ref<Camera> > cameras_c = cameras;
        for (auto it = cameras_c.begin(), _e = cameras_c.end(); it != _e; ++it) {
            Camera *camera = **it;

            TouchInput::Event &event = ei->events[0];
            const Size &size = getRenderer()->getSize();
            camera->sendTouchEvent((Object3D::EventType)event.phase, Vector2f(event.x/size.width(), event.y/size.height()));
        }
    }
}

void TouchInput::addListener(ActionCallback callback, void *data) {
    manager.push(callback, data);
}

void TouchInput::addCamera(const Ref<Camera> &camera) {
    cameras.push_back(camera);
}

void TouchInput::clear() {
    manager.clear();
    cameras.clear();
}

