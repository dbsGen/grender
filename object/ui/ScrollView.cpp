//
// Created by gen on 15/03/2017.
//

#include "ScrollView.h"
#include <Renderer.h>
#include <math/Math.hpp>
#include <utils/tween/all.h>

using namespace gr;
using namespace gc;
using namespace std;

ScrollView::ScrollView() : enable_horizontal(true),
                           enable_vertical(true) {
    setCollision(true);
    container = new View();
    add(container);
}

ScrollView::~ScrollView() {
    if (tween)
        tween->stop();
}

Vector2f vector2f_lerp(const Vector2f &v1, const Vector2f &v2, float p) {
    return lerp(v1, v2, p);
}

bool ScrollView::onMessage(const StringName &key, const RArray *vars) {
    if (key == Object3D::MESSAGE_TOUCH_EVENT) {
        variant_vector &vec = vars->vec();
        EventType type = (Object3D::EventType)((int)vec.at(0));
        Vector2f *point = vec.at(1).get<Vector2f>();
        switch (type){
            case TOUCH_BEGIN: {
                if (tween) {
                    tween->stop();
                    tween = NULL;
                }
                old_speed.x(0);
                old_speed.y(0);
                old_pos = *point;
                on_drag_down(this, (void*)true);
            }
                break;
            case TOUCH_MOVE: {
                const Size &size = Renderer::sharedInstance()->getSize();
                Vector2f off = (*point - old_pos);
                const Size &ss = getSize();
                const Size &cs = getContentSize();
                float max_x = cs.width() - ss.width();
                float max_y = cs.height() - ss.height();
                Vector2f speed = Vector2f(0, 0);
                if (enable_horizontal) {
                    speed.x(-off.x() * size.x());
                    if (offset.x() < 0 || offset.x() > max_x) {
                        speed.x(speed.x()/2);
                    }
                }
                if (enable_vertical) {
                    speed.y(-off.y() * size.y());
                    if (offset.y() < 10 || offset.y() > max_y) {
                        speed.y(speed.y()/2);
                    }
                }
                Vector2f t = offset + speed;
                old_speed = lerp(old_speed, speed, 0.2);
                setOffset(t);
                old_pos = *point;
            }
                break;
            case TOUCH_END:
            case TOUCH_CANCEL: {
                on_drag_down(this, (void*)false);
                checkBorder();
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
    return View::onMessage(key, vars);
}

void ScrollView::setOffset(const Vector2f &offset)  {
    if (this->offset != offset) {
        this->offset = offset;
        container->setPosition(Vector3f(-offset.x(), -offset.y(), 0));
        on_scroll(this, (void*)&offset);
        onScroll();
    }
}

void ScrollView::setPadding(const Vector4f &padding) {
    this->padding = padding;
    checkBorder();
}

void ScrollView::checkBorder() {
    const Size &ss = getSize();
    const Size &cs = getContentSize();
    float max_x = cs.width() - ss.width();
    float max_y = cs.height() - ss.height();
    Vector2f t = offset;
    if (enable_horizontal) {
        if (t.x() < padding.x()) {
            t.x(padding.x());
        }else if (t.x() > max_x) {
            t.x(max(max_x, padding.x()));
        }else if (fabsf(old_speed.x()) > 0.5) {
            t.x(t.x() + clamp(old_speed.x() * 20, -ss.width(), ss.width()));
            if (t.x() < padding.x()) {
                t.x(padding.x());
            }else if (t.x() > max_x) {
                t.x(max(max_x, padding.x()));
            }
        }
    }
    if (enable_vertical) {
        if (t.y() < padding.y()) {
            t.y(padding.y());
        }else if (t.y() > max_y) {
            t.y(max(max_y, padding.y()));
        } else if (fabsf(old_speed.y()) > 0.5) {
            t.y(t.y() + clamp(old_speed.y() * 20, -ss.width(), ss.width()));
            if (t.y() < padding.y()) {
                t.y(padding.y());
            }else if (t.y() > max_y) {
                t.y(max(max_y, padding.y()));
            }
        }
    }
    if (t != offset) {
        tween = Tween::New<CubicOut>(this, 0.6);
        tween->addProperty(makeProperty(&ScrollView::setOffset, &ScrollView::getOffset, offset, t, vector2f_lerp));
        tween->start();
    }
}
