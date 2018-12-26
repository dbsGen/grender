//
// Created by gen on 16/6/22.
//

#ifndef HI_RENDER_PROJECT_ANDROID_VIEW_H
#define HI_RENDER_PROJECT_ANDROID_VIEW_H

#include <mesh/Panel.h>
#include "../Object3D.h"
#include "../../render_define.h"

#define VIEW_LAYER_GAP 0.15f

namespace gr {
    CLASS_BEGIN_N(ViewMaterial, Material)

    public:
        ViewMaterial();

    CLASS_END
    CLASS_BEGIN_N(View, Object3D)

    private:
        float       border_width;
        float       corner;
        Size        size;
        Color       border_color;
        bool        depth_dirty;
        double      final_depth;
        int         depth;

        bool        alpha_changed;
        float       alpha;
        float       final_alpha;

    protected:
        virtual bool onMessage(const gc::StringName &key, const gc::RArray *vars);
        _FORCE_INLINE_ virtual void updateSize(const Vector2f &originalSize, const Vector2f &size) {};

    public:
        static const gc::StringName MESSAGE_ALPHA_CHANGED;
        static const gc::StringName MESSAGE_DEPTH_CHANGED;

        _FORCE_INLINE_ gc::Ref<Panel> getPanel() { return gc::Ref<Panel>(getMesh()); }

        METHOD _FORCE_INLINE_ const Size &getSize() {return size;}
        METHOD void setSize(const Size &size);
    
        METHOD _FORCE_INLINE_ float getBorderWidth() { return border_width; }
        METHOD void setBorderWidth(float borderWidth);
    
        METHOD _FORCE_INLINE_ float getCorner() { return corner; }
        METHOD void setCorner(float corner);
    
        METHOD _FORCE_INLINE_ const Color &getBorderColor() {return border_color;}
        METHOD void setBorderColor(const Color &color);

        METHOD _FORCE_INLINE_ int getDepth() {return depth;}
        METHOD void setDepth(int depth);

        METHOD _FORCE_INLINE_ float getAlpha() {return alpha;}
        METHOD void setAlpha(float alpha);
        float getFinalAlpha();

        double getFinalDepth();
    
        virtual void setMaterial(const gc::Ref<Material> &material);

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
