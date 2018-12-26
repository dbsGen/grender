//
// Created by gen on 16/7/11.
//

#ifndef VOIPPROJECT_TEXTINPUT_H
#define VOIPPROJECT_TEXTINPUT_H

#include "Button.h"
#include "../../render_define.h"

namespace gr {
    CLASS_BEGIN_N(TextInput, View)

    private:
        static TextInput *focusdTextInput;

        gc::Ref<Button> background;
        gc::Ref<Label>  textLabel;
        gc::Ref<Label>  placeholder;
        Size            padding;
        Label::Anchor   hAnchor;

        gc::StringName  notificationKey;

        std::string     value;

        bool            security;

        void setLabelsSize();
        void checkText();
        void setSecurityText();

    protected:
        virtual void updateSize(const Vector2f &originalSize, const Vector2f &size);

    public:
        static const gc::StringName NOTIFICATION_FOCUS_CHANGE;

        TextInput();
        ~TextInput();

        const gc::Ref<Label> &getTextLabel();
        const gc::Ref<Label> &getPlaceholderLabel();

        const gc::Ref<Button> &getBackground() {return background;}
        void setBackground(const gc::Ref<Button> &background) {this->background = background;}

        void setText(const std::string &text);
        const std::string &getText();

        void setPlaceholder(const std::string &placeholder);
        const std::string *getPlaceholder();

        void setPadding(const Size &padding);
        const Size &getPadding();

        void setHAnchor(Label::Anchor anchor);
        Label::Anchor getHAnchor();

        _FORCE_INLINE_ bool isSecurity() {return security;}
        void setSecurity(bool security);

        void focus();
        void blur();

    CLASS_END
}


#endif //VOIPPROJECT_TEXTINPUT_H
