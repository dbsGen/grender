//
// Created by gen on 16/7/11.
//

#include <utils/NotificationCenter.h>
#include <sstream>
#include <texture/ColorTexture.h>
#include <Renderer.h>
#include <platforms/Platform.h>
#include "TextInput.h"

using namespace hirender;

const StringName TextInput::NOTIFICATION_FOCUS_CHANGE("NOTIFICATION_TEXT_INPUT_FOCUS_CHANGE");
TextInput *TextInput::focusdTextInput = NULL;

TextInput::TextInput() : View(),
                         hAnchor(Label::Begin),
                         security(false) {
    background = new Button;
    background->setSize(getSize());
    background->setOnClick([](void* bg, void* send_data, void *data){
        ((TextInput*)data)->focus();
    }, this);
    background->setTexture(new ColorTexture(HColor(1,1,1,1)));
    add(background);

    notificationKey = NotificationCenter::keyFromObject(this);

    NotificationCenter::getInstance()->listen(NOTIFICATION_FOCUS_CHANGE,
                                              [](void *name, void *params, void* data){
                                                  TextInput *ti = (TextInput*)data;
                                                  if (ti != focusdTextInput) ti->checkText();
                                              }, notificationKey, this);

    NotificationCenter::getInstance()->listen(Renderer::NOTIFICATION_HANDLE_EVENT,
                                              [](void *name, void *params, void* data){
                                                  TextInput *ti = (TextInput*)data;
                                                  vector<Variant> *vs = (vector<Variant> *)params;
                                                  EventType type = (Object::EventType) ((int)vs->at(0));
                                                  if (type == TOUCH_BEGIN) {
                                                      if (focusdTextInput == ti) {
                                                          ti->blur();
                                                      }
                                                  }
                                              }, notificationKey, this);
    padding = HSize(5, 5);
}

TextInput::~TextInput() {
    NotificationCenter::getInstance()->remove(NOTIFICATION_FOCUS_CHANGE, notificationKey);
    NotificationCenter::getInstance()->remove(Renderer::NOTIFICATION_HANDLE_EVENT, notificationKey);
}

void TextInput::checkText() {
    if (focusdTextInput == this) {
        if (placeholder) placeholder->setEnable(false);
        getTextLabel()->setEnable(true);
    }else if (textLabel && textLabel->getText().size() > 0) {
        if (placeholder) placeholder->setEnable(false);
        textLabel->setEnable(true);
    }else if (placeholder && placeholder->getText().size() > 0) {
        placeholder->setEnable(true);
        if (textLabel) textLabel->setEnable(false);
    }else {
        if (placeholder) placeholder->setEnable(false);
        if (textLabel) textLabel->setEnable(false);
    }
}

const Ref<Label> &TextInput::getTextLabel() {
    if (!textLabel) {
        textLabel = new Label;
        textLabel->setColor(HColor(0,0,0,1));
        textLabel->setFont(Font::defaultFont());
        textLabel->translate(Vector3f(0,0,-VIEW_LAYER_GAP));
        textLabel->setHAnchor(hAnchor);
        add(textLabel);
        checkText();
        setLabelsSize();
    }
    return textLabel;
}

const Ref<Label> &TextInput::getPlaceholderLabel() {
    if (!placeholder) {
        placeholder = new Label;
        placeholder->setColor(HColor(0,0,0,0.2));
        placeholder->setFont(Font::defaultFont());
        placeholder->translate(Vector3f(0,0,-VIEW_LAYER_GAP));
        placeholder->setHAnchor(hAnchor);
        add(placeholder);
        checkText();
        setLabelsSize();
    }
    return placeholder;
}

void TextInput::setText(const string &text) {
    value = text;
    if (security) {
        setSecurityText();
    }else
        getTextLabel()->setText(text);
    checkText();
}

const string &TextInput::getText() {
    return value;
}

void TextInput::setPlaceholder(const string &placeholder) {
    getPlaceholderLabel()->setText(placeholder);
    checkText();
}

const string *TextInput::getPlaceholder() {
    if (placeholder) {
        return &placeholder->getText();
    }
    return NULL;
}

void TextInput::focus() {
    if (focusdTextInput != this) {
        focusdTextInput = this;
        checkText();
        NotificationCenter::getInstance()->trigger(NOTIFICATION_FOCUS_CHANGE);

        const string &text = getText();
        const string *placeholder = getPlaceholder();
        struct _Callback : public Callback {
            TextInput *input;
            _Callback (TextInput *i) {input = i;}
            Variant invoke(const Array &params) {
                int type = params.at(0);
                switch (type) {
                    case 0:
                        input->setText(params.at(1));
                        break;
                    case 1:
                        input->blur();
                        
                    default:
                        break;
                }
                input->setText(params.at(1));
                return Variant::null();
            }
        };
        Platform::sharedPlatform()->startInput(text, placeholder ? *placeholder : "", new _Callback(this));
    }
}

void TextInput::blur() {
    if (focusdTextInput == this) {
        focusdTextInput = NULL;
        NotificationCenter::getInstance()->trigger(NOTIFICATION_FOCUS_CHANGE);
    }
}

void TextInput::updateSize(const Vector2f &originalSize, const Vector2f &size) {
    background->setSize(size);
    setLabelsSize();
}

void TextInput::setPadding(const HSize &padding) {
    this->padding = padding;
    setLabelsSize();
}

const HSize &TextInput::getPadding() {
    return padding;
}

void TextInput::setLabelsSize() {
    Matrix4 pose = Matrix4::identity() + Vector3f(padding.x(), padding.y(), -VIEW_LAYER_GAP);
    HSize fs = getSize() - padding.scale(2);
    if (textLabel) {
        textLabel->setPose(pose);
        textLabel->setSize(fs);
    }
    if (placeholder) {
        placeholder->setPose(pose);
        placeholder->setSize(fs);
    }
}

void TextInput::setHAnchor(Label::Anchor anchor) {
    hAnchor = anchor;
    if (textLabel) textLabel->setHAnchor(anchor);
    if (placeholder) placeholder->setHAnchor(anchor);
}

Label::Anchor TextInput::getHAnchor() {
    return hAnchor;
}

void TextInput::setSecurity(bool security) {
    this->security = security;
    if (security) {
        setSecurityText();
    }else {
        textLabel->setText(value);
    }
}

void TextInput::setSecurityText() {
    int length = (int)value.size();
    char *chs = (char*)malloc((length+1) * sizeof(char));
    for (int i = 0; i < length; ++i) {
        chs[i] = '*';
    }
    chs[length] = 0;
    getTextLabel()->setText(chs);
    free(chs);
}
