//
//  GL2ArrayBuffer.hpp
//  hirender_iOS
//
//  Created by gen on 01/01/2017.
//  Copyright © 2017 gen. A0-'******************;/3ll rights reserved.
//

#ifndef GL2ArrayBuffer_hpp
#define GL2ArrayBuffer_hpp

#include <core/Object.h>
#include "include_opengl2.h"
#include <material/Shader.h>
#include <graphics/graphics_define.h>

namespace gr {
    class Material;
}

namespace gg {
    class GL2AttrVector;
    CLASS_BEGIN_0_N(GL2ArrayBuffer)
    
    GLuint  glID;
    
    size_t  size;
    size_t  totle_size;
    bool    single;
    
    pointer_list    attrs;
    pointer_list    pose_objects;
    
    static void bufferChanged(void *sender, void *send_data, void *data);
    static void poseChanged(void *sender, void *send_data, void *data);
    void update(GL2AttrVector *vec, size_t offset_start, size_t length);
    void updatePose(GL2AttrVector *vec, void *pose_target, size_t offset_start, size_t length);
    
public:
    _FORCE_INLINE_ GL2ArrayBuffer() : glID(0), size(0), totle_size(0) {}
    ~GL2ArrayBuffer();
    
    void bind(const gc::StringName &prop_name,
              gr::Shader::Type prop_type,
              const pointer_list &objects,
              size_t count, GLuint index);
    void clear();
    void drawIndexes(const pointer_list &objects,
                     size_t count);
    
    CLASS_END
}

#endif /* GL2ArrayBuffer_hpp */
