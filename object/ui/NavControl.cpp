//
//  NavControl.cpp
//  hirender_iOS
//
//  Created by gen on 29/03/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "NavControl.h"
#include <texture/ColorTexture.h>
#include <utils/tween/Properties.h>
#include <core/math/Math.hpp>
#include <utils/FileSystem.h>
#include "ShadowView.h"
#include <Renderer.h>

using namespace gr;

const StringName kNAV_POSITION_X("NAV_POSITION_X");
const StringName kNAV_POSITION_Y("NAV_POSITION_Y");
#define kWIDTH 38
#define kBLUR 16


Ref<Material> NavControlButton::button_material;

NavControlButton::NavControlButton() {
    if (!button_material) {
        static const char shader_str[] =
#include "nav_circle.frag.h"
        ;
        StringName sn("nav_circle.frag");
        Shader::context()->addData(sn, new BufferData((void*)shader_str, sizeof(shader_str)));
        button_material = new ViewMaterial;
        button_material->setShader(new Shader(Shader::SHADER_SIMPLE_VERT, sn));
        button_material->setTexture(new ColorTexture(HColor(1,1,1,1)), 0);
        button_material->setRenderIndex(99);
    }
    setMaterial(button_material);
    setBorderWidth(5 * Renderer::getScreenScale());
    setBorderColor(HColor(1,1,1,1));
    setCorner(kWIDTH * Renderer::getScreenScale() / 2);
    
    ShadowView *shadow = new_t(ShadowView);
    shadow->setColor(HColor(0, 0, 0,1));
    shadow->setAlpha(0.6);
    HSize size = HSize(kWIDTH + kBLUR, kWIDTH + kBLUR) * Renderer::getScreenScale();
    shadow->setSize(size);
    shadow->setPosition(Vector3f(-kWIDTH/2 - kBLUR/2, -kWIDTH/2 - kBLUR/2, 0)* Renderer::getScreenScale());
    shadow->setDepth(-1);
    shadow->setBlur(12*Renderer::getScreenScale());
    shadow->setCorner((kWIDTH + kBLUR)/2*Renderer::getScreenScale());
    shadow->setName("subnav_shadow");
    add(shadow);
}

void NavControlButton::changeSubView(const Ref<View> &sub_view, bool animate, Direction direction) {
    if (sub_view == this->sub_view) return;
    HSize s = HSize(kWIDTH - 14, kWIDTH - 14) * Renderer::getScreenScale();
    if (sub_view) {
        sub_view->setSize(s);
        const Ref<Material> &mat = sub_view->getMaterial();
        if (mat)
            mat->setRenderIndex(100);
    }
    if (animate) {
        if (this->sub_view) {
            Tween *tween = Tween::New(*this->sub_view, 0.4);
            tween->addProperty(alphaProperty(1, 0));
            switch (direction) {
                case UpDown:{
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(0,0, 0)),
                                                        HQuaternion::fromEuler(Vector3f(M_PI_2, 0, 0))));
                    tween->addProperty(positionProperty(Vector3f(0, 0, 0),
                                                        Vector3f(0, -30, 0)));
                }
                    break;
                case DownUp: {
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(0,0, 0)),
                                                        HQuaternion::fromEuler(Vector3f(-M_PI_2, 0, 0))));
                    tween->addProperty(positionProperty(Vector3f(0, 0, 0),
                                                        Vector3f(0, 30, 0)));
                }
                    break;
                case LeftRight: {
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(0,0, 0)),
                                                        HQuaternion::fromEuler(Vector3f(0, M_PI_2, 0))));
                    tween->addProperty(positionProperty(Vector3f(0, 0, 0),
                                                        Vector3f(30, 0, 0)));
                }
                    break;
                case RightLeft: {
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(0,0, 0)),
                                                        HQuaternion::fromEuler(Vector3f(0, -M_PI_2, 0))));
                    tween->addProperty(positionProperty(Vector3f(0, 0, 0),
                                                        Vector3f(-30, 0, 0)));
                }
                    break;
                    
                default:
                    break;
            }
            tween->start();
        }
        
        if (sub_view) {
            add(sub_view);
            sub_view->setAlpha(0);
            Tween *tween = Tween::New(sub_view, 0.4);
            tween->addProperty(alphaProperty(0, 1));
            switch (direction) {
                case UpDown:{
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(M_PI_2,0, 0)),
                                                        HQuaternion::fromEuler(Vector3f(0, 0, 0))));
                    tween->addProperty(positionProperty(Vector3f(0, 30, 0),
                                                        Vector3f(0, 0, 0)));
                }
                    break;
                case DownUp: {
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(-M_PI_2,0, 0)),
                                                        HQuaternion::fromEuler(Vector3f(0, 0, 0))));
                    tween->addProperty(positionProperty(Vector3f(0, -30, 0),
                                                        Vector3f(0, 0, 0)));
                }
                    break;
                case LeftRight: {
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(0,M_PI_2, 0)),
                                                        HQuaternion::fromEuler(Vector3f(0, 0, 0))));
                    tween->addProperty(positionProperty(Vector3f(-30, 0, 0),
                                                        Vector3f(0, 0, 0)));
                }
                    break;
                case RightLeft: {
                    tween->addProperty(rotationProperty(HQuaternion::fromEuler(Vector3f(0,-M_PI_2, 0)),
                                                        HQuaternion::fromEuler(Vector3f(0, 0, 0))));
                    tween->addProperty(positionProperty(Vector3f(30, 0, 0),
                                                        Vector3f(0, 0, 0)));
                }
                    break;
                    
                default:
                    break;
            }
            tween->setOnComplete(&NavControlButton::changeComplete, this);
            tween->start();
        }
        this->sub_view = sub_view;
    }else {
        if (this->sub_view) remove(this->sub_view);
        this->sub_view = sub_view;
        if (sub_view) {
            sub_view->setAlpha(1);
            add(sub_view);
        }
    }
}

void NavControlButton::changeComplete(void *sender, void *send_data, void *data) {
    NavControlButton *btn = (NavControlButton*)data;
    btn->on_swap_over(btn);
}

NavControl::NavControl() : extend(true) {
    setCollision(true);
    
    setDepth(99);
    status = 0;
    setSize(HSize(kWIDTH, kWIDTH) * Renderer::getScreenScale());
    
    main_button = new NavControlButton;
    main_button->setSize(HSize(kWIDTH, kWIDTH) * Renderer::getScreenScale());
    main_button->getMesh()->setColor(HColor(0xff9101ff));
    main_button->getPanel()->setAnchor(Vector2f(0.5, 0.5));
    main_button->setOnDrag(&NavControl::dragEvent, this);
    main_button->setOnClick(&NavControl::clickEvent, this);
    main_button->setDragMode(true);
    main_button->setDepth(2);
    main_button->pushOnSwapOver(&NavControl::mainButtonSwapOver, this);
    add(main_button);
    main_swap = false;
    
    string pos_x, pos_y;
    if (FileSystem::getInstance()->configGet(kNAV_POSITION_X, pos_x) && FileSystem::getInstance()->configGet(kNAV_POSITION_Y, pos_y)) {
        setPosition(Vector3f(atof(pos_x.c_str()), atof(pos_y.c_str()), 0));
    }else {
        setPosition(Vector3f(0, 300, 0));
    }
    
    arrow_view = new_t(View);
    View *view = new_t(View);
    static const char shader_str[] =
#include <object/ui/arrow.frag>
    ;
    StringName name("arrow.frag");
    Shader::context()->addData(name, new BufferData((void*)shader_str, sizeof(shader_str)));
    view->setMaterial(new Material(new Shader(Shader::SHADER_SIMPLE_VERT, name)));
    view->getMaterial()->setRenderIndex(100);
    view->getMaterial()->setBlend(true);
    view->setSize(HSize(7, 14) * Renderer::getScreenScale());
    view->getMesh()->setColor(HColor(1,1,1,1));
    view->getPanel()->setAnchor(Vector2f(0.5, 0.5));
    view->setPosition(Vector3f(-1, 0, 0) * Renderer::getScreenScale());
    arrow = view;
    arrow->getMaterial()->setCullFace(Material::Both);
    HSize s = HSize(kWIDTH - 14, kWIDTH - 14) * Renderer::getScreenScale();
    arrow_view->setSize(s);
    arrow_view->getMesh()->cast_to<Panel>()->setAnchor(Vector2f(0.5, 0.5));
    arrow_view->add(view);
}

void NavControl::copyObject(Ref<View> &target, const Ref<View> &object) {
    if (!target) {
        target = new View;
        ViewMaterial *mat = new ViewMaterial;
        mat->setRenderIndex(99);
        target->setMaterial(mat);
    }
    if (target->getMaterial() != object->getMaterial()) {
        target->getMaterial()->copy(object->getMaterial());
        target->setMaterial(target->getMaterial());
    }
    if (target->getMesh() != object->getMesh())
        target->getMesh()->copy(object->getMesh());
}

void NavControl::checkStatus() {
    const HSize &size = getRenderer()->getSize();
    const Vector3f &pos = getPosition();
    int s = 0;
    if (pos.x() < size.width() / 2) {
        s |= 0x1;
    }else {
        s |= 0x2;
    }
    if (pos.y() < 80 * Renderer::getScreenScale()) {
        s |= 0x4;
    }else if (pos.y() > size.height() - (60 * Renderer::getScreenScale())) {
        s |= 0x8;
    }
    setStatus(s);
}

void NavControl::setStatus(int status) {
    if (this->status != status) {
        this->status = status;
        int count = 0;
        for (auto it = display_buttons.begin(),
             _e = display_buttons.end(); it != _e; ++it) {
            const Ref<Button> &button = *it;
            Tween::cancel(*button);
            Tween *tween = Tween::New<CubicOut>(*button, 0.6f);
            TweenProperty *pro = poseProperty(button->getPose(), positionAt(count++, (int)display_buttons.size(), status));
            tween->addProperty(pro);
            tween->start();
        }
        if (status & 0x1) {
            Tween::cancel(*arrow);
            Tween *tween = Tween::New<CubicOut>(*arrow, 0.4);
            tween->addProperty(rotationProperty(arrow->getRotation(), HQuaternion::fromEuler(Vector3f(0, M_PI, 0))));
            tween->addProperty(positionProperty(arrow->getPosition(), Vector3f(-1, 0, 0) * Renderer::getScreenScale()));
            tween->start();
        }else {
            Tween::cancel(*arrow);
            Tween *tween = Tween::New<CubicOut>(*arrow, 0.4);
            tween->addProperty(rotationProperty(arrow->getRotation(), HQuaternion::fromEuler(Vector3f(0, 0, 0))));
            tween->addProperty(positionProperty(arrow->getPosition(), Vector3f(1, 0, 0) * Renderer::getScreenScale()));
            tween->start();
        }
    }
}

void NavControl::touchBorder() {
    const HSize &size = getRenderer()->getSize();
    const Vector3f &pos = getPosition();
    Vector3f target;
    if (pos.x() < size.x() / 2) {
        target.x((kWIDTH/2 + 5) * Renderer::getScreenScale());
        touch_left = true;
    }else {
        target.x(size.width() - (kWIDTH/2 + 5) * Renderer::getScreenScale());
        touch_left = false;
    }
    if (pos.y() < 0) {
        target.y((kWIDTH/2 + 5) * Renderer::getScreenScale());
    }else if (pos.y() > size.y() - getSize().height()) {
        target.y(size.y() - (kWIDTH/2 + 5) * Renderer::getScreenScale());
    }else {
        target.y(pos.y());
    }
    char pos_chs[10];
    sprintf(pos_chs, "%.2f", target.x());
    FileSystem::getInstance()->configSet(kNAV_POSITION_X, pos_chs);
    sprintf(pos_chs, "%.2f", target.y());
    FileSystem::getInstance()->configSet(kNAV_POSITION_Y, pos_chs);
    Tween *tween = Tween::New<CircularOut>(this, 0.3);
    TweenProperty *pro = makeProperty(&NavControl::setPosition, &NavControl::getPosition, pos, target);
    tween->addProperty(pro);
    tween->start();
}

void NavControl::awake(Renderer *renderer) {
    checkStatus();
    touchBorder();
}

void NavControl::onDragBegin(const Vector2f &off) {
    old_position = off;
}

void NavControl::onDragEnd(const Vector2f &off) {
    touchBorder();
}

void NavControl::onDrag(const Vector2f &off) {
//    if (!extend) {
        Vector2f v2 = off - old_position;
        setPosition(getPosition() + Vector3f(v2.x(), v2.y(), 0));
        old_position = off;
    checkStatus();
//    }
}

void NavControl::onMissOver(void *sender, void *send_data, void *data) {
    NavControl *nav = (NavControl*)data;
    nav->on_missing(data);
//    button->setEnable(false);
}


void NavControl::display() {
    if (!extend) {
        extend = true;
        int count = 0;
        
        for (auto it = display_buttons.begin(), _e = display_buttons.end(); it != _e; ++it) {
            const Ref<Button> &button = *it;
            button->setEnable(true);
            Tween::cancel(*button);
            Tween *tween = Tween::New<BackOut>(*button, 0.4f);
            int off = count;
            button->setDepth(off == 0 ? 1 : 0);
            tween->setDelay(0.03 * off);
            TweenProperty *pro = makeProperty(&Button::setPose, &Button::getPose, button->getPose(), positionAt(count++, (int)display_buttons.size(), status), &Matrix4::lerp);
            tween->addProperty(pro);
            tween->addProperty(alphaProperty(button->getAlpha(), 1.0));
            tween->start();
        }
    }
}

void NavControl::miss() {
    if (extend) {
        extend = false;
        int count = 0, size = (int)display_buttons.size();
        if (size) {
            for (auto it = display_buttons.begin(), _e = display_buttons.end(); it != _e; ++it) {
                const Ref<Button> &button = *it;
                Tween::cancel(*button);
                Tween *tween = Tween::New<BackIn>(*button, 0.4f);
                int off = count;
                tween->setDelay(0.03 * off);
                TweenProperty *pro = makeProperty(&Button::setPosition, &Button::getPosition, button->getPosition(), Vector3f(0, 0, 0));
                tween->addProperty(pro);
                pro = makeProperty(&Button::setScale, &Button::getScale, button->getScale(), Vector3f::one() * 0.8);
                tween->addProperty(pro);
                pro = makeProperty(&Button::setRotation,
                                   &Button::getRotation,
                                   button->getRotation(),
                                   HQuaternion::fromEuler(Vector3f(M_PI_2, 0, 0)));
                tween->addProperty(pro);
                tween->addProperty(alphaProperty(button->getAlpha(), 0.0));
                if (count == size - 1) {
                    tween->setOnComplete(NavControl::onMissOver, this);
                }
                tween->start();
                ++count;
            }
        }else {
            onMissOver(NULL, NULL, this);
        }
    }
}

void NavControl::onButtonClicked(NavControlButton *button) {
    if (button->getAlpha() < 0.01 || main_swap) return;
    if (button == *main_button) {
        if (currentCount() > 0) {
            Ref<NavControlItem> item = items.back();
            if (main_button->getSubView() == arrow_view) {
                NavControlItem::NavButtonIndex idx = -1;
                item->on_clickd(this, &idx);
            }
        }else {
            on_main_clicked(this);
        }
    }else {
        if (!extend) return;
        int idx = 0;
        for (auto it = display_buttons.begin(), _e = display_buttons.end(); it != _e; ++it) {
            const Ref<NavControlButton> &cb = *it;
            if (*cb == button) {
                const Ref<NavControlItem> &item = items.back();
                if (item->current_index != idx) {
                    int old = item->current_index;
                    item->current_index = idx;
                    item->on_clickd(this, &idx);
                    main_swap = true;
                    copyObject(swap_icon, item->sub_icons[item->getCurrentIndex()]);
                    main_button->changeSubView(swap_icon, true, idx > old ? NavControlButton::UpDown : NavControlButton::DownUp);
                }
                break;
            }
            ++idx;
        }
    }
}

Matrix4 NavControl::positionAt(int idx, int total, int type) {
    float start_alpha = 0;
    float delta = M_PI * 2;
    if (type & 0x1) {
        if (type & 0x4) {
            start_alpha = M_PI_2;
            delta = M_PI_2;
        }else if (type & 0x8) {
            delta = M_PI_2;
        }else {
            delta = M_PI;
        }
    }else if (type & 0x2) {
        if (type & 0x4) {
            start_alpha = -M_PI_2;
            delta = -M_PI_2;
        }else if (type & 0x8) {
            delta = -M_PI_2;
            start_alpha = 0;
        }else {
            delta = -M_PI;
            start_alpha = 0;
        }
    }else {
        if (type & 0x4) {
            start_alpha = M_PI_2;
            delta = M_PI;
        }else if (type & 0x8) {
            start_alpha = -M_PI_2;
            delta = M_PI;
        }
    }
    
    float pa = delta / (total - 1);
    float alpha = start_alpha + idx * pa;
    if (fabs(fabs(delta) - M_PI) < 0.01) {
        pa = delta / total;
        alpha = start_alpha + pa / 2 + idx * pa;
    }
    float x = sinf(alpha) * 130;
    float y = -cosf(alpha) * 130;
    Matrix4 m4 = Matrix4::identity() + Vector3f(x, y, 0);
    return m4;
}

void NavControl::processButton(const Ref<NavControlButton> &button, int idx) {
    button->setPose(positionAt(idx, (int)display_buttons.size(), 0));
}

void NavControl::dragEvent(void *sender, void *send_data, void *data) {
    NavControl *nav = (NavControl*)data;
    ButtonEvent *be = (ButtonEvent*)send_data;
    switch (be->getStatus()) {
        case 1:
            nav->onDragBegin(be->getPosition());
            break;
        case 2:
            nav->onDrag(be->getPosition());
            break;
        case 3:
            nav->onDragEnd(be->getPosition());
            break;
            
        default:
            break;
    }
}

void NavControl::clickEvent(void *sender, void *send_data, void *data) {
    NavControl *nav = (NavControl*)data;
    nav->onButtonClicked((NavControlButton*)sender);
}

void NavControl::pushOnMainClicked(ActionCallback callback, void *data) {
    on_main_clicked.push(callback, data);
}

void NavControl::removeOnMainClicked(ActionCallback callback, void *data) {
    on_main_clicked.remove(callback, data);
}

void NavControl::pushItem(const Ref<NavControlItem> &item, bool animate) {
    items.push_back(item);
    reload(animate);
}

Ref<NavControlItem> NavControl::popItem(bool animate) {
    Ref<NavControlItem> item = items.back();
    items.pop_back();
    reload(animate);
    return item;
}

void NavControl::removeSubButton(const Ref<NavControlButton> &button) {
    cache_buttons.push_back(button);
    remove(button);
    display_buttons.remove(button);
}

Ref<NavControlButton> NavControl::popSubButton() {
    if (cache_buttons.size()) {
        Ref<Button> btn = cache_buttons.back();
        cache_buttons.pop_back();
        return btn;
    }
    return Ref<Button>::null();
}

Ref<NavControlButton> NavControl::makeSubButton() {
    Ref<NavControlButton> sub = popSubButton();
    if (!sub) {
        sub = new NavControlButton;
        sub->setSize(HSize(kWIDTH, kWIDTH) * Renderer::getScreenScale());
        sub->getMesh()->setColor(HColor(1,0,0,1));
        sub->getPanel()->setAnchor(Vector2f(0.5, 0.5));
        sub->setOnClick(&NavControl::clickEvent, this);
    }
    return sub;
}

void NavControl::reloadAnimated() {
    auto buttons = display_buttons;
    for (auto it = buttons.begin(), _e = buttons.end(); it != _e; ++it) {
        removeSubButton(*it);
    }
    const Ref<NavControlItem> &item = items.back();
    for (size_t i = 0, e = item->sub_icons.size(); i < e; ++i) {
        const Ref<View> &view = item->sub_icons[i];
        if (view) {
            Ref<NavControlButton> sub = makeSubButton();
            sub->setPose(Matrix4::identity() * (Vector3f::one() * 0.8));
            sub->setAlpha(0);
            sub->changeSubView(view, false);
            add(sub);
            display_buttons.push_back(sub);
        }
    }
    const Ref<View> &view = item->sub_icons[item->getCurrentIndex()];
    if (view) {
        copyObject(swap_icon, view);
        main_button->changeSubView(swap_icon, true, NavControlButton::LeftRight);
    }else {
        main_button->changeSubView(arrow_view, true, NavControlButton::LeftRight);
    }
    display();
}

void NavControl::reloadMissingCallback(void *sender, void *send_data, void *data) {
    NavControl *nc = (NavControl*)sender;
    nc->reloadAnimated();
    nc->on_missing.callback = NULL;
}

void NavControl::reload(bool animate) {
    if (animate) {
        on_missing.callback = &NavControl::reloadMissingCallback;
        miss();
    }else {
        auto buttons = display_buttons;
        for (auto it = buttons.begin(), _e = buttons.end(); it != _e; ++it) {
            removeSubButton(*it);
        }
        const Ref<NavControlItem> &item = items.back();
        for (size_t i = 0, e = item->sub_icons.size(); i < e; ++i) {
            Ref<NavControlButton> sub = makeSubButton();
            if (extend) {
                sub->setPose(positionAt((int)i, (int)e, 0));
            } else {
                sub->setPose(Matrix4::identity() * (Vector3f::one() * 0.8));
                sub->setAlpha(0);
            }
            sub->changeSubView(item->sub_icons[i], animate);
            add(sub);
            display_buttons.push_back(sub);
        }
        copyObject(display_icon, item->sub_icons[item->getCurrentIndex()]);
        main_button->changeSubView(display_icon, false);
    }
}

void NavControl::mainButtonSwapOver(void *sender, void *send_data, void *data) {
    NavControl *nav = (NavControl *)data;
    Ref<View> tmp = nav->display_icon;
    nav->display_icon = nav->swap_icon;
    nav->swap_icon = tmp;
    nav->main_swap = false;
}

bool NavControl::onMessage(const StringName &key, const Array *vars) {
    if (extend) {
        if (key == Object::MESSAGE_TOUCH_EVENT) {
            variant_vector *vec = vars->vec();
            EventType type = (Object::EventType)((int)vec->at(0));
            Vector2f *point = vec->at(1).get<Vector2f>();
            switch (type){
                case TOUCH_BEGIN: {
//                    if (tween) {
//                        tween->stop();
//                        tween = NULL;
//                    }
//                    old_speed = 0;
//                    old_pos = *point;
                }
                    break;
                case TOUCH_MOVE: {
//                    const HSize &size = Renderer::sharedInstance()->getSize();
//                    Vector2f off = (*point - old_pos);
//                    const HSize &ss = getSize();
//                    float speed = - off.height() * size.height() / ss.height() * 2;
//                    old_speed = lerp(old_speed, speed, 0.2f);
//                    
//                    old_pos = *point;
                }
                    break;
                case TOUCH_END:
                case TOUCH_CANCEL: {
//                    const HSize &ss = getSize();
//                    float target = clamp(old_speed * 20, -ss.width(), ss.width());
//                    target = floorf(target + 0.5f);
//                    const Ref<NavControlItem> &item = items.back();
//                    size_t item_count = item->sub_icons.size();
//                    if (target < 0) {
//                        target = 0;
//                    }else if (target > item_count - 1) {
//                        target = item_count - 1;
//                    }
//                    
//                    if (current_position != target) {
//                        tween = Tween::New<CubicOut>(this, 0.6);
//                        tween->addProperty(makeProperty(&NavControl::setCurrentPosition, &NavControl::getCurrentPosition, current_position, target));
//                        tween->start();
//                    }
                    //checkMiss();
                }
                    break;
                case TOUCH_MOVE_OUT: {
                }
                    break;
                case TOUCH_MOVE_IN: {
                }
                    break;
                default:
                    break;
            }
        }
    }
    return View::onMessage(key, vars);
}
