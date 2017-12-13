//
// Created by gen on 16/6/1.
//

#ifndef HIGRAPHICS_FACTORY_H
#define HIGRAPHICS_FACTORY_H

#include <core/Singleton.h>
#include "graphics_define.h"

using namespace gcore;

namespace higraphics {
    CLASS_BEGIN_TN(Factory, Singleton, 1, Factory)

    public:
        enum GraphicsAPI {
            GLES2,
            METAL,
            VULKAN
        };
    private:
        GraphicsAPI api;

    public:
    
        static const gcore::HClass *mesh_class,
                                    *texture_class,
                                    *material_class,
                                    *shader_class,
                                    *renderer_class,
                                    *shader_compiler_class;

        /**
         * The graphics api, default GLES2
         */
        _FORCE_INLINE_ GraphicsAPI getAPI() {return api;}
        _FORCE_INLINE_ void setAPI(GraphicsAPI api) {this->api = api;}

        /**
         * Create a instance
         * @params cls, the class you want to create.
         *      MeshIMP::getClass()
         *      TextureIMP::getClass()
         *      ShaderIMP::getClass()
         *      MaterialIMP::getClass()
         *      RendererIMP::getClass()
         * @result A instance of cls.
         */
        HObject *create(const gcore::HClass *cls);

        Factory();

    CLASS_END
}


#endif //HIGRAPHICS_FACTORY_H
