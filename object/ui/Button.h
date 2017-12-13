//
// Created by gen on 16/7/11.
//

#ifndef VOIPPROJECT_BUTTON_H
#define VOIPPROJECT_BUTTON_H

#include <core/Action.h>
#include "ImageView.h"
#include "Label.h"
#include "../../render_define.h"

namespace gr {
    
    CLASS_BEGIN_0_N(ButtonEvent)
    int         status;
    Vector2f    position;
public:
    
    _FORCE_INLINE_ void setStatus(int s) {
        status = s;
    }
    _FORCE_INLINE_ int getStatus() {
        return status;
    }
    
    _FORCE_INLINE_ void setPosition(const Vector2f &pos) {
        position = pos;
    }
    _FORCE_INLINE_ const Vector2f &getPosition() {
        return position;
    }
    
    CLASS_END
    
    CLASS_BEGIN_N(Button, ImageView)

    public:
        enum Status {
            Normal,
            Press,
            Focus,
#define StatusEnd ((int)Button::Focus)
            All,
        };
    private:
        struct StatusInfo {
            string          text;
            Ref<Texture>    background;
            HColor          textColor;
            Vector3f        scale;
        };

        bool            touch_begin;
        bool            focus_in;
        bool            drag_mode;
        bool            drag_begin;
        bool            set_hover_scale;
        bool            event_pop;
        bool            can_click;
        ActionItem      on_click;
        ActionItem      on_drag;
        Ref<Label>      label;
        StatusInfo      statuses[StatusEnd+1];
        HSize           padding;
        Status          status;
        HSize           render_size;
        Vector2f        old_pos;
        int             id;
        string          voice_command;

        void setStatus(Status status);

    protected:
        virtual bool onMessage(const StringName &key, const Array *vars);
        virtual void updateSize(const Vector2f &originalSize, const Vector2f &size);
        virtual void _setStatus(Status status){};
        virtual void onHover(){};
        virtual void onNormal(){};
        virtual void onPress(){};

    public:
        /**
         * send_data: ButtonEvent*
         */
        _FORCE_INLINE_ void setOnClick(ActionCallback callback, void *data = NULL) {
            on_click.callback = callback;
            on_click.data = data;
        }


        _FORCE_INLINE_ void setVoiceCommand(const char *command){
            voice_command = command;
        }
        /**
         * send_data: ButtonEvent*
         */
        _FORCE_INLINE_ void setOnDrag(ActionCallback callback, void *data = NULL) {
            on_drag.callback = callback;
            on_drag.data = data;
        }
    
        _FORCE_INLINE_ void setDragMode(bool mode) {
            drag_mode = mode;
        }
        _FORCE_INLINE_ bool getDragMode() {
            return drag_mode;
        }

        _FORCE_INLINE_ void enableEventPop(bool enable) {
            event_pop = enable;
        }
        _FORCE_INLINE_ bool getEventPop() {
            return event_pop;
        }

        _FORCE_INLINE_ void setPadding(const HSize &padding) {this->padding = padding;}
        _FORCE_INLINE_ const HSize &getPadding() {return padding;}

        _FORCE_INLINE_ Status getStatus() {return status;}

        _FORCE_INLINE_ void setCanClick(bool can_click){this->can_click = can_click;}

        void setText(const string &text, Status status = All);
        const string *getText(Status status = Normal);

        const string *getVoiceCommand(Status status = Normal);

        void setId(int id){this->id = id;}
        int getId(){ return id;}

        void setBackground(const Ref<Texture> &texture, Status status = All);
        const Ref<Texture> &getBackground(Status status = Normal);

        void setHoverScale(const Vector3f &scale,Status status=All);

//        void setHoverListener(ActionCallback hover_callback,void *data=NULL);

        void setTextColor(const HColor color, Status status = All);
        const HColor &getTextColor(Status status = All);

        const Ref<Label> &getLabel();

        Button();
        ~Button();

    CLASS_END
}


#endif //VOIPPROJECT_BUTTON_H
