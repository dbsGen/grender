//
// Created by gen on 16/6/22.
//

#ifndef HI_RENDER_PROJECT_ANDROID_IMAGEVIEW_H
#define HI_RENDER_PROJECT_ANDROID_IMAGEVIEW_H

#include "View.h"
#include "../../render_define.h"

namespace gr {
    CLASS_BEGIN_N(ImageView, View)

    public:
        ImageView();

        METHOD void setTexture(const gc::Ref<Texture> &texture);
        METHOD gc::Ref<Texture> getTexture();
    
        void makePixelPerfect();

    protected:
        ON_LOADED_BEGIN(cls, View)
            ADD_METHOD(cls, ImageView, setTexture);
            ADD_METHOD(cls, ImageView, getTexture);
        ON_LOADED_END
    CLASS_END
}


#endif //HI_RENDER_PROJECT_ANDROID_IMAGEVIEW_H
