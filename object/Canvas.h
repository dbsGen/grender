//
//  Canvas.hpp
//  hirender_iOS
//
//  Created by mac on 2017/1/22.
//  Copyright © 2017年 gen. All rights reserved.
//

#ifndef Canvas_hpp
#define Canvas_hpp

#include "Object.h"
#include "../texture/RenderTexture.h"
#include "../render_define.h"

namespace gr {
    class RenderTexture;
    CLASS_BEGIN_N(Canvas, Object)
    
    Ref<RenderTexture> texture;
    pointer_map change_flags;
    
    static void vertexUpdate(void *sender, void *send_data, void *data);
    void processMesh(const Ref<Mesh> &mesh);
    void vertexChanged();
    
public:
    Canvas();
    
    virtual void onPoseChanged();
    virtual void setMesh(const Ref<Mesh> &mesh);
    
    void willRender(Camera *camera);
    
    _FORCE_INLINE_ const Ref<RenderTexture> &getTexture() {
        return texture;
    }
    
    CLASS_END
}


#endif /* Canvas_hpp */
