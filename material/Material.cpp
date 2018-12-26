//
// Created by gen on 16/5/30.
//

#include "Material.h"
#include <object/ui/View.h>
#include <graphics/Factory.h>
#include <texture/ColorTexture.h>

using namespace gr;
using namespace gg;
using namespace gc;

const StringName Material::DEFAULT_PROJECTION("PROJECTION");
const StringName Material::DEFAULT_VIEW("VIEW");
const StringName Material::DEFAULT_TRANSLATE("MODEL");
const StringName Material::DEFAULT_TEXTURE("TEXTURE");
const StringName Material::DEFAULT_POSITION("A_POSITION");
const StringName Material::DEFAULT_COLOR("A_COLOR");
const StringName Material::DEFAULT_UV("A_TEX_COORD");

Ref<Material> Material::color_material;
Ref<Material> Material::view_material;

const Ref<Material> &Material::colorMaterial() {
    if (!color_material) {
        color_material = new Material(Shader::defaultShader());
        color_material->setTexture(new ColorTexture(Color(1,1,1,1)), 0);
    }
    return color_material;
}

const Ref<Material> &Material::viewMaterial() {
    if (!color_material) {
        color_material = new ViewMaterial;
        color_material->setTexture(new ColorTexture(Color(1,1,1,1)), 0);
    }
    return color_material;
}

Material::Material() : cull_face(Both), render_index(0), blend(false) {
    compile_changed = true;
    depth = false;
    imp = (MaterialIMP*)Factory::getInstance()->create(MaterialIMP::getClass());
    imp->_setTarget(this);
    
    position_attribute = NULL;
//    colorAttribute = NULL;
//    uvAttribute = NULL;
    view_uniform = NULL;
    projection_uniform = NULL;
    translate_uniform = NULL;
    
    projection = Matrix4::identity();
    view = Matrix4::identity();
}

Material::~Material() {
    delete(imp);
}

void Material::process() {
    preProcess();
    if (compile_changed) {
        shader->compile(compile_values);
        imp->updateShader(*shader);
        compile_changed = false;
    }
    imp->process();
}

void Material::setUniform(const StringName &name, const Variant &obj) {
    uniform_values[(void*)name] = obj;
}
void Material::setUniform(const Shader::Property *property, const Variant &obj) {
    if (property) {
        setUniform(property->name, obj);
    }
}

void Material::setPositionAttribute(const Shader::Property *pa) {
    if (pa != position_attribute) {
        position_attribute = pa;
        imp->cleanPosition();
    }
}

void Material::setCompileValue(const StringName &name, const Variant &var) {
    compile_values[name] = var;
    compile_changed = true;
}

void Material::setProjectionUniform(const Shader::Property *pu) {
    projection_uniform = pu;
    if (pu) setProjection(projection);
}

void Material::setViewUniform(const Shader::Property *vu) {
    view_uniform = vu;
    if (vu) setView(view);
}

void Material::setTranslateUniform(const Shader::Property *tu) {
    if (tu != translate_uniform) {
        translate_uniform = tu;
        imp->cleanTranslate();
    }
}

void Material::setShader(const Ref<Shader> &s) {
    if (shader != s) {
        shader = s;
        const Shader *shaderPtr = *shader;
        //uniform_values.clear();
        if (!s) {
            setTranslateUniform(NULL);
            setProjectionUniform(NULL);
            setViewUniform(NULL);
            setPositionAttribute(NULL);
        }else {
            setTranslateUniform(shaderPtr->uniform(DEFAULT_TRANSLATE));
            setProjectionUniform(shaderPtr->uniform(DEFAULT_PROJECTION));
            setViewUniform(shaderPtr->uniform(DEFAULT_VIEW));
            setPositionAttribute(shaderPtr->attribute(DEFAULT_POSITION));
        }
        compile_changed = true;
    }
}

void Material::_copy(const Object *other) {
    const Material *mat = other->cast_to<Material>();
    if (mat) {
        setShader(mat->shader);
        for (auto it = mat->uniform_values.begin(), _e = mat->uniform_values.end(); it != _e; ++it) {
            uniform_values[it->first] = it->second;
        }
        projection = mat->projection;
    }
}

const Ref<Shader> &Material::getShader() {
    if (!shader) {
        setShader(Shader::defaultShader());
    }
    return shader;
}

void Material::setProjection(const Matrix4 &p) {
    if (projection_uniform) {
        setUniform(projection_uniform->name, p);
    }
    projection = p;
}

void Material::setView(const Matrix4 &v) {
    if (view_uniform) {
        setUniform(view_uniform->name, v);
    }
    view = v;
}

void Material::setTexture(const Ref<Texture> &tex, int index) {
    setUniform(getShader()->uniform(Shader::Sampler, index), Variant(tex));
}
const Ref<Texture> Material::getTexture(int index) {
    return getUniformRef<Texture>(getShader()->uniform(Shader::Sampler, index)->name);
}
