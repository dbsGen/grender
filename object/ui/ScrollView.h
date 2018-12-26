//
// Created by gen on 15/03/2017.
//

#ifndef NEXTLAUNCHER_GIT_SCROLLVIEW_H
#define NEXTLAUNCHER_GIT_SCROLLVIEW_H

#include "View.h"
#include <utils/tween/all.h>
#include <render_define.h>

namespace gr {
    CLASS_BEGIN_N(ScrollView, View)

        gc::Ref<View> container;
        Vector2f offset;
        Vector2f old_pos;
        Vector2f old_speed;
        gc::Ref<Tween> tween;

        bool enable_horizontal;
        bool enable_vertical;
    
        gc::ActionManager on_scroll;
        gc::ActionManager on_drag_down;
        Vector4f padding;
    
        void checkBorder();

    protected:
        virtual bool onMessage(const gc::StringName &key, const gc::RArray *vars);

        _FORCE_INLINE_ virtual void onScroll() {}

    public:
        _FORCE_INLINE_ const gc::Ref<View> &getContainer() {
            return container;
        }
        _FORCE_INLINE_ void setContentSize(const Size &size) {
            container->setSize(size);
        }
        _FORCE_INLINE_ const Size &getContentSize() {
            return container->getSize();
        }
        void setOffset(const Vector2f &offset);
        _FORCE_INLINE_ const Vector2f &getOffset() {
            return offset;
        }

        _FORCE_INLINE_ bool getEnableHorizontal() {
            return enable_horizontal;
        }
        _FORCE_INLINE_ void setEnableHorizontal(bool enable) {
            enable_horizontal = enable;
        }
        _FORCE_INLINE_ bool getEnableVertical() {
            return enable_vertical;
        }
        _FORCE_INLINE_ void setEnableVertical(bool enable) {
            enable_vertical = enable;
        }
    
        _FORCE_INLINE_ void addOnScroll(gc::ActionCallback callback, void *data) {
            on_scroll.push(callback, data);
        }
        _FORCE_INLINE_ void addOnDragDown(gc::ActionCallback callback, void *data) {
            on_drag_down.push(callback, data);
        }
    
        void setPadding(const Vector4f &padding);
        const Vector4f &getPadding() {
            return padding;
        }
    
        ScrollView();
        ~ScrollView();

    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_SCROLLVIEW_H
