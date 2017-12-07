//
// Created by gen on 16/6/22.
//

#ifndef HI_RENDER_PROJECT_ANDROID_VIEW_H
#define HI_RENDER_PROJECT_ANDROID_VIEW_H

#include <mesh/Panel.h>
#include "../Object.h"
#include "../../render_define.h"

#define VIEW_LAYER_GAP 0.15f

namespace hirender {
    CLASS_BEGIN_N(ViewMaterial, Material)

    public:
        ViewMaterial();

    CLASS_END
    CLASS_BEGIN_N(View, Object)

    private:
        float       border_width;
        float       corner;
        HSize       size;
        HColor      border_color;
        bool        depth_dirty;
        double      final_depth;
        int         depth;

        bool        alpha_changed;
        float       alpha;
        float       final_alpha;

    protected:
        virtual bool onMessage(const StringName &key, const Array *vars);
        _FORCE_INLINE_ virtual void updateSize(const Vector2f &originalSize, const Vector2f &size) {};

    public:
        static const StringName MESSAGE_ALPHA_CHANGED;
        static const StringName MESSAGE_DEPTH_CHANGED;

        _FORCE_INLINE_ Ref<Panel> getPanel() { return Ref<Panel>(getMesh()); }

        METHOD _FORCE_INLINE_ const HSize &getSize() {return size;}
        METHOD void setSize(const HSize &size);
    
        METHOD _FORCE_INLINE_ float getBorderWidth() { return border_width; }
        METHOD void setBorderWidth(float borderWidth);
    
        METHOD _FORCE_INLINE_ float getCorner() { return corner; }
        METHOD void setCorner(float corner);
    
        METHOD _FORCE_INLINE_ const HColor &getBorderColor() {return border_color;}
        METHOD void setBorderColor(const HColor &color);

        METHOD _FORCE_INLINE_ int getDepth() {return depth;}
        METHOD void setDepth(int depth);

        METHOD _FORCE_INLINE_ float getAlpha() {return alpha;}
        METHOD void setAlpha(float alpha);
        float getFinalAlpha();

        double getFinalDepth();
    
        virtual void setMaterial(const Ref<Material> &material);

        View();

    protected:
        ON_LOADED_BEGIN(cls, Object)
            ADD_METHOD(cls, View, getSize);
            ADD_METHOD(cls, View, setSize);
            ADD_METHOD(cls, View, getBorderWidth);
            ADD_METHOD(cls, View, setBorderWidth);
            ADD_METHOD(cls, View, getCorner);
            ADD_METHOD(cls, View, setCorner);
            ADD_METHOD(cls, View, getBorderColor);
            ADD_METHOD(cls, View, setBorderColor);
            ADD_METHOD(cls, View, getDepth);
            ADD_METHOD(cls, View, setDepth);
            ADD_METHOD(cls, View, getAlpha);
            ADD_METHOD(cls, View, setAlpha);
        ON_LOADED_END
    CLASS_END
}


#endif //HI_RENDER_PROJECT_ANDROID_VIEW_
