//
//  Label.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HI_RENDER_LABEL_H
#define HI_RENDER_LABEL_H

#include "View.h"
#include <font/Font.h>
#include "../../render_define.h"

namespace hirender {
    CLASS_BEGIN_N(Label, View)

    public:
        enum Anchor {
            Begin,
            Center,
            End
        };

    private:
        Vector2f    _offset;

        Ref<Font>   font;
        string      text;
        HSize       space;
        float       max_width;
        HColor      color;
        ref_vector  characters;
        ref_map     materials;
        float       point_size;
        bool        ellipsis;

        pointer_map     cache_characters;
        Reference popChar(const Ref<Character> &ch);
        Reference popChar(unsigned long ch);
        void pushChar(const Reference &ch);
        void clearCachedChars();
        void pushAllToCache();
        HSize text_size;

        bool changed;
        StringName notification_key;

        Anchor h_anchor;
        Anchor v_anchor;

        void prevFrame();

        void makeLine(const pointer_vector *chs, float lineWidth, float totalHeight);

    protected:
        virtual bool onMessage(const StringName &key, const Array *vars);
        virtual void awake(Renderer *renderer);
        virtual void updateSize(const Vector2f &originalSize, const Vector2f &size);

    public:
        Label();
        ~Label();

        _FORCE_INLINE_ const HSize &getTextSize() {
            if (changed) {
                refresh();
            }
            return text_size;
        }

        _FORCE_INLINE_ void setPointSize(float size) {
            point_size = size;
        }
        _FORCE_INLINE_ float getPointSize() {
            return point_size;
        }
    
        _FORCE_INLINE_ bool getEllipsis() {
            return ellipsis;
        }
        void setEllipsis(bool ellipsis);
    
        void setFont(const Ref<Font> &font);
        const Ref<Font> &getFont() const;

        void setText(const string &text);
        const string &getText() const;

        void setSpace(const HSize &space);
        const HSize &getSpace();

        void setMaxWidth(float maxWidth);
        float getMaxWidth();

        void setColor(const HColor &color);
        const HColor &getColor();

        Anchor getHAnchor();
        void setHAnchor(Anchor anchor);

        Anchor getVAnchor();
        void setVAnchor(Anchor anchor);

        void refresh();

    CLASS_END
}

#endif /* HI_RENDER_LABEL_H */
