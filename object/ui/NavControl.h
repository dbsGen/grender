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

namespace hirender {
    CLASS_BEGIN_N(NavControlItem, RefObject)
    
    Ref<View> main_icon;
    vector<Ref<View> > sub_icons;
    int default_index;
    int current_index;
    ActionManager on_clickd;
    
    friend class NavControl;
    
public:
    typedef int NavButtonIndex;
    static const NavButtonIndex MainButton = -1;
    
    _FORCE_INLINE_ void setMainIcon(const Ref<View> &icon) {
        main_icon = icon;
    }
    _FORCE_INLINE_ const Ref<View> &getMainIcon() {
        return main_icon;
    }
    
    _FORCE_INLINE_ void pushSubIcon(const Ref<View> &icon) {
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
    _FORCE_INLINE_ void pushOnClicked(ActionCallback callback, void *data) {
        on_clickd.push(callback, data);
    }
    _FORCE_INLINE_ void removeOnClicked(ActionCallback callback, void *data) {
        on_clickd.remove(callback, data);
    }
    
    NavControlItem() : default_index(0), current_index(-1) {
    }
    
    CLASS_END
    
    CLASS_BEGIN_N(NavControlButton, Button)
    
    static Ref<Material> button_material;
    Ref<View> sub_view;
    ActionManager on_swap_over;
    static void changeComplete(void *sender, void *send_data, void *data);
    
public:
    enum Direction {
        UpDown,
        DownUp,
        LeftRight,
        RightLeft
    };
    void changeSubView(const Ref<View> &sub_view, bool animate, Direction direction = LeftRight);
    _FORCE_INLINE_ void pushOnSwapOver(ActionCallback callback, void *data) {
        on_swap_over.push(callback, data);
    }
    _FORCE_INLINE_ void removeOnSwapOver(ActionCallback callback, void *data) {
        on_swap_over.remove(callback, data);
    }
    _FORCE_INLINE_ const Ref<View> &getSubView() {
        return sub_view;
    }
    NavControlButton();
    
    CLASS_END
    
    CLASS_BEGIN_N(NavControl, View)
    
    Ref<View>   swap_icon;
    Ref<View>   display_icon;
    Ref<NavControlButton>   main_button;
    list<Ref<NavControlItem> > items;
    
    list<Ref<NavControlButton> > display_buttons;
    list<Ref<NavControlButton> > cache_buttons;
    
    Ref<Tween>  tween;
    Ref<Object> home_icon;
    Ref<View>   arrow;
    Ref<View>   arrow_view;
    Vector2f    old_position;
    bool        touch_left;
    bool        extend;
    bool        main_swap;
    int         status;
    ActionItem  on_missing;
    
    Matrix4 positionAt(int idx, int total, int type);
    void checkStatus();
    void setStatus(int);
    
    void processButton(const Ref<NavControlButton> &button, int idx);
    
    void removeSubButton(const Ref<NavControlButton> &button);
    Ref<NavControlButton> popSubButton();
    Ref<NavControlButton> makeSubButton();
    
    _FORCE_INLINE_ int currentCount() {
        return (int)items.back()->sub_icons.size();
    }
    _FORCE_INLINE_ int limit(int input) {
        return max(min(input, currentCount() - 1), 0);
    }
    
    void reload(bool animate);
    void reloadAnimated();
    static void reloadMissingCallback(void *sender, void *send_data, void *data);
    
    ActionManager on_main_clicked;
    
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
    
    void copyObject(Ref<View> &target, const Ref<View> &object);
    
protected:
    void awake(Renderer *renderer);
    bool onMessage(const StringName &key, const Array *vars);
    
public:
    NavControl();
    
    void pushOnMainClicked(ActionCallback callback, void *data);
    void removeOnMainClicked(ActionCallback callback, void *data);
    
    void pushItem(const Ref<NavControlItem> &item, bool animate);
    Ref<NavControlItem> popItem(bool animate);
    
    CLASS_END
}

#endif /* NavControl_hpp */
