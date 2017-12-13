//
// Created by gen on 16/7/11.
//

#include <texture/ColorTexture.h>
#include <Renderer.h>
#include <utils/tween/all.h>
#include "Button.h"

using namespace gr;

bool Button::onMessage(const StringName &key, const Array *vars) {
    if (key == Object::MESSAGE_TOUCH_EVENT) {
        variant_vector *vec = vars->vec();
        EventType type = (Object::EventType)((int)vec->at(0));
        Vector2f *point = vec->at(1).get<Vector2f>();
        switch (type){
            case TOUCH_BEGIN:
                setStatus(Press);
                touch_begin = true;
                old_pos = *point;
                render_size = getRenderer()->getSize();
                if (!event_pop) {
                    return false;
                }
                break;
            case TOUCH_END: {
                if (touch_begin&&can_click)
                    on_click(this);
//                if(focus_in)setStatus(Focus);
            }
            case TOUCH_CANCEL: {
                touch_begin = false;
                if(!focus_in)
                setStatus(Normal);
                else
                    setStatus(Focus);
                if (drag_begin) {
                    ButtonEvent bv;
                    bv.setPosition(Vector2f(point->x() * render_size.x(), point->y() * render_size.y()));
                    bv.setStatus(3);
                    drag_begin = false;
                    on_drag(this, &bv);
                }
                if (!event_pop) {
                    return false;
                }
            }
                break;
            case TOUCH_MOVE_OUT: {
                if (!drag_mode) {
                    touch_begin = false;
                }
                setStatus(Normal);
            }
                break;
            case TOUCH_MOVE_IN: {
                if (!drag_mode) {
                    touch_begin = true;
                }
                setStatus(Press);
            }
                break;
            case TOUCH_MOVE: {
                if (touch_begin && drag_mode && (point->operator-(old_pos)).length() > 0.01) {
                    touch_begin = false;
                }
                ButtonEvent bv;
                bv.setPosition(Vector2f(point->x() * render_size.x(), point->y() * render_size.y()));
                if (!drag_begin) {
                    bv.setStatus(1);
                    drag_begin = true;
                }else {
                    bv.setStatus(2);
                }
                on_drag(this, &bv);
                if (!event_pop) {
                    return false;
                }
            }
                break;
            case  FOCUS_IN: {
                if (getStatus() == Normal) {
                    setStatus(Focus);
                    focus_in = true;
                }
            }
                break;
            case FOCUS_OUT: {
                if (getStatus() == Focus) {
                    setStatus(Normal);
                    focus_in = false;
                }
            }
                break;
            default:
                break;
        }
    }
    return ImageView::onMessage(key, vars);
}

void Button::updateSize(const Vector2f &originalSize, const Vector2f &size) {
    if (label) label->setSize(size);
}

Button::Button() : ImageView(),
                   focus_in(false),
                   status(Normal),
                   touch_begin(false),
                   drag_mode(false),
                   set_hover_scale(false),
                   drag_begin(false),
                   event_pop(false), can_click(true){
    setCollision(true);
//    setTextColor(HColor(0,0,0,1));
    setBackground(new ColorTexture(HColor(1,1,1,1)));
}

const Ref<Label> &Button::getLabel() {
    if (!label) {
        label = new Label();
        label->setFont(Font::defaultFont());
        label->setSize(getSize());
        label->translate(Vector3f(padding.x(), padding.y(), 0));
        label->setColor(getTextColor());
        label->setHAnchor(Label::Center);
        label->setVAnchor(Label::Center);
        label->setPointSize(16);
        add(label);
    }
    return label;
}

void Button::setText(const string &text, Status status) {
    if (status == All) {
        for (int i = 0; i <= StatusEnd; ++i) {
            setText(text, (Status) i);
        }
    }else {
        statuses[status].text = text;
        if (status == this->status)
        {
            setTextColor(HColor(0,0,0,1));
            getLabel()->setText(text);
        }
    }
}

void Button::setBackground(const Ref<Texture> &texture, Button::Status status) {
    if (status == All) {
        for (int i = 0; i <= StatusEnd; ++i) {
            setBackground(texture, (Status) i);
        }
    }else {
        statuses[status].background = texture;
        if (status == this->status) setTexture(texture);
    }
}

const Ref<Texture>& Button::getBackground(Status status) {
    return statuses[status].background;
}

void Button::setTextColor(const HColor color, Status status) {
    if (status == All) {
        for (int i = 0; i <= StatusEnd; ++i) {
            setTextColor(color, (Status) i);
        }
    }else {
        statuses[status].textColor = color;
        if (status == this->status)
            getLabel()->setColor(color);
    }
}

const HColor& Button::getTextColor(Status status) {
    return statuses[status].textColor;
}

const string *Button::getText(Status status) {
    if (status > StatusEnd)
        status = this->status;
    return &statuses[status].text;
}

void Button::setHoverScale(const Vector3f &scale, Status status) {
    set_hover_scale = true;
    if (status == All) {
        for (int i = 0; i <= StatusEnd; ++i) {
            setHoverScale(scale, (Status) i);
        }
    }else {
        statuses[status].scale = scale;
        if (status == this->status){
            setScale(scale);
        }
    }
}

void Button::setStatus(Status status) {
    _setStatus(status);

    if (this->status != status) {
        if (status > StatusEnd)
            status = this->status;
        this->status = status;
        switch(status){
            case Normal:{
                onNormal();
            }
                break;
            case Press:{
                onPress();
            }
                break;
            case Focus:{
                onHover();
            }
                break;
            default:
                break;

        }
        const StatusInfo &info = statuses[status];
        const string &text = info.text;
        if (!text.empty()) {
            getLabel()->setText(text);
            getLabel()->setColor(info.textColor);
        }
        setTexture(info.background);
        if(set_hover_scale){
            float delta_x = getSize().x()*(getScale().x()-info.scale.x())/2;
            float delta_y = getSize().y()*(getScale().y()-info.scale.y())/2;
            setPosition(getPosition()+Vector3f(delta_x,delta_y,0));
            setScale(info.scale);
        }
        change();
    }
}

Button::~Button() {
    LOG(i, "Button delete");
}
