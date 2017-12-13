//
//  Panel.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/2.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIRENDER_PANEL_H
#define HIRENDER_PANEL_H

#include "Mesh.h"
#include <render_define.h>

namespace gr {
    CLASS_BEGIN_N(Panel, Mesh)
    
private:
    Vector2f size;
    Vector2f anchor;
    void updateVertexes();
    void createVertexes();
    
protected:
    virtual void _copy(const HObject *other);
    
public:
    
    Panel();
    INITIALIZE(Panel, PARAMS(float w, float h),
               size.x(w);
               size.y(h);
               updateVertexes();
               )
    
    float getWidth();
    void setWidth(float width);
    float getHeight();
    void setHeight(float height);
    const Vector2f &getSize();
    void setSize(Vector2f size);
    
    void setAnchor(const Vector2f &anchor);
    _FORCE_INLINE_ const Vector2f &getAnchor() const {
        return anchor;
    }
    
    CLASS_END
}

#endif //HIRENDER_PANEL_H
