//
// Created by gen on 16/7/7.
//

#include <sstream>
#include <Renderer.h>
#include "NotificationCenter.h"

using namespace gr;

void NotificationCenter::listen(const StringName &name,
                                ActionCallback function,
                                const StringName &key, void* data) {
    mtx.lock();
    NotificationItem *item = new NotificationItem(key, function, data);
    auto items = listeners.find(name);
    if (items == listeners.end()) {
        pointer_list *l = new pointer_list;
        l->push_back(item);
        listeners[(void*)name] = l;
    }else
        ((pointer_list*)items->second)->push_back(item);
    mtx.unlock();
}

void NotificationCenter::listen(const StringName &name, const RefCallback &callback) {
    mtx.lock();
    NotificationItem *item = new NotificationItem(callback);
    auto items = listeners.find(name);
    if (items == listeners.end()) {
        pointer_list *l = new pointer_list;
        l->push_back(item);
        listeners[(void*)name] = l;
    }else
        ((pointer_list*)items->second)->push_back(item);
    mtx.unlock();
}

void NotificationCenter::trigger(const StringName &name, vector<Variant> *params) {
    mtx.lock();
    auto items = listeners.find(name);
    if (items != listeners.end()) {
        pointer_list vs = *(pointer_list*)items->second;
        mtx.unlock();
        for (auto it = vs.begin(), _e = vs.end(); it != _e; ++it) {
            NotificationItem *item = (NotificationCenter::NotificationItem *)*it;
            if (item->listener) item->listener(&item->key, (void*)params, item->data);
            if (item->callback) item->callback->invoke(*params);
        }
    }else {
        mtx.unlock();
    }
}


void NotificationCenter::remove(const StringName &name,
                                const StringName &key) {
    mtx.lock();
    auto items = listeners.find(name);
    if (items != listeners.end()) {
        pointer_list* vs = (pointer_list*)items->second;

        for (auto it = vs->begin(), _e = vs->end(); it != _e; ++it) {
            NotificationItem *item = (NotificationCenter::NotificationItem *)*it;
            if (item->key == key) {
                vs->erase(it);
                delete item;
                break;
            }
        }
    }
    mtx.unlock();
}

void NotificationCenter::remove(const StringName &name, const RefCallback &callback) {
    mtx.lock();
    auto items = listeners.find(name);
    if (items != listeners.end()) {
        pointer_list* vs = (pointer_list*)items->second;
        
        for (auto it = vs->begin(), _e = vs->end(); it != _e; ++it) {
            NotificationItem *item = (NotificationCenter::NotificationItem *)*it;
            if (item->callback == callback) {
                vs->erase(it);
                delete item;
                break;
            }
        }
    }
    mtx.unlock();
}

StringName NotificationCenter::keyFromObject(HObject *object) {
    stringstream ss;
    ss << '[' << object->getInstanceClass()->getFullname() << ": " << (long)object << ']';
    return StringName(ss.str().c_str());
}

NotificationCenter::~NotificationCenter() {
    for (auto it = listeners.begin(), _e = listeners.end(); it != _e; ++it) {
        pointer_list* vs = (pointer_list*)it->second;
        for (auto vit = vs->begin(), _ve = vs->end(); vit != _ve; ++vit) {
            delete (NotificationCenter::NotificationItem *)*vit;
        }
        delete vs;
    }
}
