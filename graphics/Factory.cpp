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

using namespace higraphics;
using namespace gr;

Factory::Factory() {
    api = GLES2;

    GL2MeshIMP::getClass();
    GL2TextureIMP::getClass();
    GL2ShaderIMP::getClass();
    GL2MaterialIMP::getClass();
    GL2RendererIMP::getClass();
    GL2ShaderCompilerIMP::getClass();
}

const HClass *Factory::mesh_class = MeshIMP::getClass();
const HClass *Factory::texture_class = TextureIMP::getClass();
const HClass *Factory::shader_class = ShaderIMP::getClass();
const HClass *Factory::material_class = MaterialIMP::getClass();
const HClass *Factory::renderer_class = RendererIMP::getClass();
const HClass *Factory::shader_compiler_class = ShaderCompilerIMP::getClass();

HObject *Factory::create(const HClass *cls) {
#define SELECT_CLASS(MESH, TEXTURE, SHADER, MATERIAL, RENDERER, COMPILER) (cls == mesh_class ? (HObject*)MESH : \
(cls == texture_class ? (HObject*)TEXTURE : \
(cls == shader_class ? (HObject*)SHADER : \
(cls == material_class ? (HObject*)MATERIAL : \
(cls == renderer_class ? (HObject*)RENDERER : \
(cls == shader_compiler_class ? (HObject*)COMPILER : NULL))))))

#define new_class(CLASS) new_c(ClassDB::getInstance()->find(#CLASS), CLASS)
    
//    LOG(i, "Init %s \n", cls->getFullname().c_str());
    switch (api) {
        case GLES2:
        {
            //const HClass *cls = ClassDB::getInstance()->find("higraphics::GL2MeshIMP");
        }
            return SELECT_CLASS(new_class(higraphics::GL2MeshIMP),
                                new_class(higraphics::GL2TextureIMP),
                                new_class(higraphics::GL2ShaderIMP),
                                new_class(higraphics::GL2MaterialIMP),
                                new_class(higraphics::GL2RendererIMP),
                                new_class(higraphics::GL2ShaderCompilerIMP));
            break;
            
        default:
            break;
    }
    return NULL;
#undef new_class
#undef SELECT_CLASS
}
