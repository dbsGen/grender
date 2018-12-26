//
// Created by gen on 28/11/2016.
//

#ifndef NEXTLAUNCHER_LOOPTHREAD_H
#define NEXTLAUNCHER_LOOPTHREAD_H

#include <thread>
#include <atomic>
#include <condition_variable>
#include <core/Object.h>
#include <core/Action.h>

#include "../render_define.h"

namespace gr {

    CLASS_BEGIN_0_N(LoopThread)

    private:
        bool w;
        std::thread *t;
        gc::ActionItem i;
        gc::ActionItem on_start;
        gc::ActionItem on_over;

        void threadProcess();

    protected:
        virtual void _cancel() {}

    public:
        _FORCE_INLINE_ LoopThread() : w(false) {}
        LoopThread(gc::ActionCallback callback, void *data);
        ~LoopThread();
        _FORCE_INLINE_ bool running() {
            return !!t;
        }

        void start();
        void cancel();

        void setOnStart(gc::ActionCallback callback, void *data) {
            on_start.callback = callback;
            on_start.data = data;
        }
        void setOnOver(gc::ActionCallback callback, void *data) {
            on_over.callback = callback;
            on_over.data = data;
        }
    CLASS_END

    CLASS_BEGIN_N(NotifyThread, LoopThread)

        std::atomic<bool> notified;
        std::condition_variable queue_check;
        std::mutex mtx;
        gc::ActionItem i;

        static void handleNotify(void *sender, void *send_data, void *data);

    protected:
        void _cancel();

    public:
        NotifyThread(){notified = false;}
        NotifyThread(gc::ActionCallback callback, void *data);
        ~NotifyThread();

        void notify();
        _FORCE_INLINE_ bool ready() {
            return !notified;
        }

    CLASS_END
}


#endif //NEXTLAUNCHER_LOOPTHREAD_H
