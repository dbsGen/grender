//
// Created by gen on 28/11/2016.
//

#include "LoopThread.h"

using namespace gr;
using namespace gc;
using namespace std;

LoopThread::LoopThread(ActionCallback callback, void *data) : w(true),
                                                              i(callback, data),
                                                              t(NULL) {
}

LoopThread::~LoopThread() {
    if (w) cancel();
    if (t) delete t;
}

void LoopThread::threadProcess() {
    on_start(this);
    while (w) {
        i(this, NULL);
    }
    on_over(this);
}

void LoopThread::start() {
    w = true;
    if (t) {
        cancel();
        delete t;
    }
    t = new thread(&LoopThread::threadProcess, this);
}

void LoopThread::cancel() {
    if (t && w) {
        w = false;
        _cancel();
        t->join();
    }
}

NotifyThread::NotifyThread(ActionCallback callback, void *data) : LoopThread(NotifyThread::handleNotify, this),
                                                                  i(callback, data) {
    notified = false;
}

NotifyThread::~NotifyThread() {
    cancel();
}

void NotifyThread::handleNotify(void *sender, void *send_data, void *data) {
    NotifyThread *nt = (NotifyThread*)data;
    unique_lock<mutex> locker(nt->mtx);
    while (!nt->notified) {
        nt->queue_check.wait(locker);
    }
    nt->i(nt, NULL);
    nt->notified = false;
}

void NotifyThread::notify() {
    unique_lock<mutex> locker(mtx);
    notified = true;
    queue_check.notify_one();
}

void NotifyThread::_cancel() {
    notify();
}



