//
// Created by gen on 16/7/7.
//

#ifndef VOIPPROJECT_NOTIFICATIONCENTER_H
#define VOIPPROJECT_NOTIFICATIONCENTER_H

#include <core/Singleton.h>
#include <core/Action.h>
#include <core/Callback.h>
#include <vector>
#include <core/Callback.h>
#include "../render_define.h"


#define IMPLEMENT_NOTIFICATION(CLASS, KEY) const StringName CLASS::KEY(#CLASS #KEY)

namespace gr {
    CLASS_BEGIN_TN(NotificationCenter, gc::Singleton, 1, NotificationCenter)

    private:
        struct NotificationItem {
            gc::StringName key;
            gc::ActionCallback listener;
            gc::RCallback callback;
            void *data;

            NotificationItem(const gc::StringName &key,
                             gc::ActionCallback listener, void* data) {
                this->key = key;
                this->listener = listener;
                this->data = data;
            }
            NotificationItem(const gc::RCallback &callback) {
                listener = NULL;
                data = NULL;
                this->callback = callback;
            }
            NotificationItem(const NotificationItem &other) {
                key = other.key;
                listener = other.listener;
                data = other.data;
                callback = other.callback;
            }
            NotificationItem &operator=(const NotificationItem &other) {
                key = other.key;
                listener = other.listener;
                data = other.data;
                callback = other.callback;
                return *this;
            }
        };

        pointer_map listeners;
        std::mutex mtx;

    public:
        NotificationCenter(){}
        ~NotificationCenter();

        /**
         * @params function((StringName*)key, (vector<Variant>*)params, customor_data)
         */
        void listen(const gc::StringName &name,
                    gc::ActionCallback function,
                    const gc::StringName &key, void *data = NULL);
        METHOD void listen(const gc::StringName &name, const gc::RCallback &callback);
        void trigger(const gc::StringName &name, variant_vector *params = NULL);
        void remove(const gc::StringName &name, const gc::StringName &key);
        METHOD void remove(const gc::StringName &name, const gc::RCallback &callback);
        static gc::StringName keyFromObject(gc::Object *object);

    protected:
        ON_LOADED_BEGIN(cls, Singleton<NotificationCenter>)
            ADD_METHOD_E(cls, NotificationCenter, void(NotificationCenter::*)(const gc::StringName &name, const gc::RCallback &callback), listen);
            ADD_METHOD_E(cls, NotificationCenter, void(NotificationCenter::*)(const gc::StringName &name, const gc::RCallback &callback), remove);
        ON_LOADED_END
    CLASS_END
}


#endif //VOIPPROJECT_NOTIFICATIONCENTER_H
