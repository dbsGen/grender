//
// Created by gen on 16/6/1.
//

#include <texture/Texture.h>
#include <mesh/Mesh.h>
#include "Factory.h"
#include <Renderer.h>
#include "opengl2/GL2MeshIMP.h"
#include "opengl2/GL2MaterialIMP.h"
#include "opengl2/GL2ShaderIMP.h"
#include "opengl2/GL2TextureIMP.h"
#include "opengl2/GL2RendererIMP.h"
#include "opengl2/GL2ShaderCompilerIMP.h"

using namespace gg;
using namespace gr;
using namespace gc;

Factory::Factory() {
    api = GLES2;

    GL2MeshIMP::getClass();
    GL2TextureIMP::getClass();
    GL2ShaderIMP::getClass();
    GL2MaterialIMP::getClass();
    GL2RendererIMP::getClass();
    GL2ShaderCompilerIMP::getClass();
}

const Class *Factory::mesh_class = MeshIMP::getClass();
const Class *Factory::texture_class = TextureIMP::getClass();
const Class *Factory::shader_class = ShaderIMP::getClass();
const Class *Factory::material_class = MaterialIMP::getClass();
const Class *Factory::renderer_class = RendererIMP::getClass();
const Class *Factory::shader_compiler_class = ShaderCompilerIMP::getClass();

Object *Factory::create(const Class *cls) {
#define SELECT_CLASS(MESH, TEXTURE, SHADER, MATERIAL, RENDERER, COMPILER) (cls == mesh_class ? (Object*)MESH : \
(cls == texture_class ? (Object*)TEXTURE : \
(cls == shader_class ? (Object*)SHADER : \
(cls == material_class ? (Object*)MATERIAL : \
(cls == renderer_class ? (Object*)RENDERER : \
(cls == shader_compiler_class ? (Object*)COMPILER : NULL))))))

#define new_class(CLASS) new_c(ClassDB::getInstance()->find(#CLASS), CLASS)
    
//    LOG(i, "Init %s \n", cls->getFullname().c_str());
    switch (api) {
        case GLES2:
        {
            //const HClass *cls = ClassDB::getInstance()->find("higraphics::GL2MeshIMP");
        }
            return SELECT_CLASS(new gg::GL2MeshIMP,
                                new gg::GL2TextureIMP,
                                new gg::GL2ShaderIMP,
                                new gg::GL2MaterialIMP,
                                new gg::GL2RendererIMP,
                                new gg::GL2ShaderCompilerIMP);
            break;
            
        default:
            break;
    }
    return NULL;
#undef new_class
#undef SELECT_CLASS
}
