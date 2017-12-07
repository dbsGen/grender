//
// Created by gen on 16/7/11.
//

#ifndef VOIPPROJECT_TEXTINPUT_H
#define VOIPPROJECT_TEXTINPUT_H

#include "Button.h"
#include "../../render_define.h"

namespace hirender {
    CLASS_BEGIN_N(TextInput, View)

    private:
        static TextInput *focusdTextInput;

        Ref<Button>     background;
        Ref<Label>      textLabel;
        Ref<Label>      placeholder;
        HSize           padding;
        Label::Anchor   hAnchor;

        StringName notificationKey;

        string value;

        bool security;

        void setLabelsSize();
        void checkText();
        void setSecurityText();

    protected:
        virtual void updateSize(const Vector2f &originalSize, const Vector2f &size);

    public:
        static const StringName NOTIFICATION_FOCUS_CHANGE;

        TextInput();
        ~TextInput();

        const Ref<Label> &getTextLabel();
        const Ref<Label> &getPlaceholderLabel();

        const Ref<Button> &getBackground() {return background;}
        void setBackground(const Ref<Button> &background) {this->background = background;}

        void setText(const string &text);
        const string &getText();

        void setPlaceholder(const string &placeholder);
        const string *getPlaceholder();

        void setPadding(const HSize &padding);
        const HSize &getPadding();

        void setHAnchor(Label::Anchor anchor);
        Label::Anchor getHAnchor();

        _FORCE_INLINE_ bool isSecurity() {return security;}
        void setSecurity(bool security);

        void focus();
        void blur();

    CLASS_END
}


#endif //VOIPPROJECT_TEXTINPUT_H
