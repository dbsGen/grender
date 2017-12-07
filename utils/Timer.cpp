//
// Created by gen on 16/9/20.
//

#include "Timer.h"

using namespace hirender;

TimerManager *TimerManager::instance(NULL);
mutex TimerManager::mtx;

TimerManager *TimerManager::sharedInstance() {
    mtx.lock();
    TimerManager *ret = instance;
    mtx.unlock();
    return instance;
}

TimerManager::TimerManager() {
    mtx.lock();
    instance = this;
    mtx.unlock();
}

TimerManager::~TimerManager() {
    mtx.lock();
    if (instance == this) {
        instance = NULL;
    }
    mtx.unlock();
}

Timer *TimerManager::fire(Time time, ActionCallback action, void *data) {
    mtx.lock();
    Timer *timer(new Timer(time, action, data));
    timers.push_back(timer);
    mtx.unlock();
    return timer;
}

void Timer::cancel() {
    TimerManager::sharedInstance()->cancel(this);
}

Timer::Timer(Time time, ActionCallback fun, void *data) {
    this->max_life = this->life = time;
    this->callback.data = data;
    this->callback.callback = fun;
}

bool Timer::step(Time delta) {
    life -= delta;
    if (life <= 0) {
        callback(this);
        return true;
    }
    return false;
}

void TimerManager::fixedStep(Renderer *renderer, Time delta) {
    auto cp = timers;
    auto it = cp.begin();
    while (it != cp.end()) {
        if (((Timer*)**it)->step(delta)) {
            timers.remove(*it);
        }
        ++it;
    }
}

void TimerManager::cancel(Timer *timer) {
    auto it = timers.begin();
    while (it != timers.end()) {
        if (timer == *it) {
            it = timers.erase(it);
        }else ++it;
    }
}
