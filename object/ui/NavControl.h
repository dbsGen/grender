//
//  NavControl.hpp
//  hirender_iOS
//
//  Created by gen on 29/03/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#ifndef NavControl_hpp
#define NavControl_hpp

#include <object/ui/View.h>
#include <object/ui/Button.h>
#include <utils/Timer.h>
#include <utils/tween/Tween.h>
#include "../../render_define.h"

namespace gr {
    CLASS_BEGIN_N(NavControlItem, gc::RefObject)
    
    gc::Ref<View> main_icon;
    std::vector<gc::Ref<View> > sub_icons;
    int default_index;
    int current_index;
    gc::ActionManager on_clickd;
    
    friend class NavControl;
    
public:
    typedef int NavButtonIndex;
    static const NavButtonIndex MainButton = -1;
    
    _FORCE_INLINE_ void setMainIcon(const gc::Ref<View> &icon) {
        main_icon = icon;
    }
    _FORCE_INLINE_ const gc::Ref<View> &getMainIcon() {
        return main_icon;
    }
    
    _FORCE_INLINE_ void pushSubIcon(const gc::Ref<View> &icon) {
        sub_icons.push_back(icon);
    }
    
    _FORCE_INLINE_ void clearSubIcons() {
        sub_icons.clear();
    }
    
    _FORCE_INLINE_ void setDefaultIndex(int idx) {
        default_index = idx;
    }
    _FORCE_INLINE_ int getDefaultIndex() {
        return default_index;
    }
    _FORCE_INLINE_ int getCurrentIndex() {
        if (current_index < 0) current_index = default_index;
        return current_index;
    }
    _FORCE_INLINE_ void pushOnClicked(gc::ActionCallback callback, void *data) {
        on_clickd.push(callback, data);
    }
    _FORCE_INLINE_ void removeOnClicked(gc::ActionCallback callback, void *data) {
        on_clickd.remove(callback, data);
    }
    
    NavControlItem() : default_index(0), current_index(-1) {
    }
    
    CLASS_END
    
    CLASS_BEGIN_N(NavControlButton, Button)
    
    static gc::Ref<Material> button_material;
    gc::Ref<View> sub_view;
    gc::ActionManager on_swap_over;
    static void changeComplete(void *sender, void *send_data, void *data);
    
public:
    enum Direction {
        UpDown,
        DownUp,
        LeftRight,
        RightLeft
    };
    void changeSubView(const gc::Ref<View> &sub_view, bool animate, Direction direction = LeftRight);
    _FORCE_INLINE_ void pushOnSwapOver(gc::ActionCallback callback, void *data) {
        on_swap_over.push(callback, data);
    }
    _FORCE_INLINE_ void removeOnSwapOver(gc::ActionCallback callback, void *data) {
        on_swap_over.remove(callback, data);
    }
    _FORCE_INLINE_ const gc::Ref<View> &getSubView() {
        return sub_view;
    }
    NavControlButton();
    
    CLASS_END
    
    CLASS_BEGIN_N(NavControl, View)
    
    gc::Ref<View>   swap_icon;
    gc::Ref<View>   display_icon;
    gc::Ref<NavControlButton>   main_button;
    std::list<gc::Ref<NavControlItem> > items;
    
    std::list<gc::Ref<NavControlButton> > display_buttons;
    std::list<gc::Ref<NavControlButton> > cache_buttons;
    
    gc::Ref<Tween>  tween;
    gc::Ref<Object> home_icon;
    gc::Ref<View>   arrow;
    gc::Ref<View>   arrow_view;
    Vector2f    old_position;
    bool        touch_left;
    bool        extend;
    bool        main_swap;
    int         status;
    gc::ActionItem  on_missing;
    
    Matrix4 positionAt(int idx, int total, int type);
    void checkStatus();
    void setStatus(int);
    
    void processButton(const gc::Ref<NavControlButton> &button, int idx);
    
    void removeSubButton(const gc::Ref<NavControlButton> &button);
    gc::Ref<NavControlButton> popSubButton();
    gc::Ref<NavControlButton> makeSubButton();
    
    _FORCE_INLINE_ int currentCount() {
        return (int)items.back()->sub_icons.size();
    }
    _FORCE_INLINE_ int limit(int input) {
        return std::max(std::min(input, currentCount() - 1), 0);
    }
    
    void reload(bool animate);
    void reloadAnimated();
    static void reloadMissingCallback(void *sender, void *send_data, void *data);
    
    gc::ActionManager on_main_clicked;
    
    static void dragEvent(void *sender, void *send_data, void *data);
    static void clickEvent(void *sender, void *send_data, void *data);
    static void onMissOver(void *sender, void *send_data, void *data);
    
    void onDrag(const Vector2f &off);
    void onDragEnd(const Vector2f &off);
    void onDragBegin(const Vector2f &off);
    void touchBorder();
    void onButtonClicked(NavControlButton *button);
    
    void display();
    void miss();
    
    static void mainButtonSwapOver(void *sender, void *send_data, void *data);
    
    void copyObject(gc::Ref<View> &target, const gc::Ref<View> &object);
    
protected:
    void awake(Renderer *renderer);
    bool onMessage(const gc::StringName &key, const gc::RArray *vars);
    
public:
    NavControl();
    
    void pushOnMainClicked(gc::ActionCallback callback, void *data);
    void removeOnMainClicked(gc::ActionCallback callback, void *data);
    
    void pushItem(const gc::Ref<NavControlItem> &item, bool animate);
    gc::Ref<NavControlItem> popItem(bool animate);
    
    CLASS_END
}

#endif /* NavControl_hpp */
