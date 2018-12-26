//
//  ShadowView.hpp
//  hirender_iOS
//
//  Created by mac on 2017/5/1.
//  Copyright © 2017年 gen. All rights reserved.
//

#ifndef ShadowView_hpp
#define ShadowView_hpp

#include "View.h"

namespace gr {
    CLASS_BEGIN(ShadowView, View)
    
    static gc::Ref<Material> shadow_material;
    static const gc::StringName ATTR_BLUR;
    static const gc::StringName ATTR_CORNER;
    static const gc::StringName ATTR_SIZE_WIDTH;
    static const gc::StringName ATTR_SIZE_HEIGHT;
    
protected:
    void updateSize(const Vector2f &originalSize, const Vector2f &size);
public:
    ShadowView();
    void setColor(const Color &color);
    void setBlur(float blur);
    void setCorner(float corner);
    
    CLASS_END
}

#endif /* ShadowView_hpp */
