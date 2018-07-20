//
// Created by gen on 16/5/31.
//

#include "Shader.h"
#include <graphics/Factory.h>
#include "ShaderParser.h"

using namespace gr;
using namespace higraphics;

namespace gr {
    typedef int VariantType;
    static const VariantType VARIANT_IDX = 16;
    _FORCE_INLINE_ void setVariantCount(VariantType &type, int count) {
        type = (type & 0xffff) + ((count - 1) << VARIANT_IDX);
    }
    _FORCE_INLINE_ int getVariantCount(VariantType type) {
        return (type >> VARIANT_IDX) + 1;
    }
    _FORCE_INLINE_ VariantType getVariantType(VariantType type) {
        return type & 0xffff;
    }
}

const StringName Shader::SHADER_TEXT_VERT("text.vert");
const StringName Shader::SHADER_TEXT_FRAG("text.frag");
const StringName Shader::SHADER_UI_FRAG("ui.frag");
const StringName Shader::SHADER_SIMPLE_VERT("simple.vert");
const StringName Shader::SHADER_SIMPLE_FRAG("simple.frag");
const StringName Shader::SHADER_IMAGE_FRAG("image.frag");
const StringName Shader::SHADER_MODEL_VERT("model.vert");
const StringName Shader::SHADER_MODEL_FRAG("model.frag");
const StringName Shader::SHADER_SHADOW_FRAG("shadow.frag");
const StringName Shader::SHADER_SHADOW_VERT("shadow.vert");

Shader::Shader() {
    imp = (ShaderIMP*)Factory::getInstance()->create(ShaderIMP::getClass());
    imp->_setTarget(this);
}

Shader::~Shader() {
    delete imp;
    clear();
}

mutex Shader::mtx;
Ref<Shader> Shader::__default_shader;

const Ref<Shader> &Shader::defaultShader() {
    mtx.lock();
    if (!__default_shader) {
        __default_shader = new Shader(SHADER_SIMPLE_VERT, SHADER_SIMPLE_FRAG);
    }
    mtx.unlock();
    return __default_shader;
}

Ref<Shader> Shader::__ui_shader;

const Ref<Shader> &Shader::uiShader() {
    mtx.lock();
    if (!__ui_shader) {
        __ui_shader = new Shader(SHADER_SIMPLE_VERT, SHADER_UI_FRAG);
    }
    mtx.unlock();
    return __ui_shader;
}

Ref<Shader> Shader::__model_shader;
const Ref<Shader>& Shader::modelShader() {
    mtx.lock();
    if (!__model_shader) {
        __model_shader = new Shader(SHADER_MODEL_VERT, SHADER_MODEL_FRAG);
    }
    mtx.unlock();
    return __model_shader;
}

Ref<ShaderContext> Shader::shader_context;
const Ref<ShaderContext> &Shader::context() {
    if (!shader_context) {
        shader_context = new ShaderContext;
        {
            static const char shader_string[] =
#include <shaders/text.vert>
            ;
            shader_context->addData(SHADER_TEXT_VERT, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/text.frag>
            ;
            shader_context->addData(SHADER_TEXT_FRAG, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/UI.frag>
            ;
            shader_context->addData(SHADER_UI_FRAG, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/simple.vert>
            ;
            shader_context->addData(SHADER_SIMPLE_VERT, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/simple.frag>
            ;
            shader_context->addData(SHADER_SIMPLE_FRAG, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/image.frag>
            ;
            shader_context->addData(SHADER_IMAGE_FRAG, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/model.vert>
            ;
            shader_context->addData(SHADER_MODEL_VERT, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/model.frag>
            ;
            shader_context->addData(SHADER_MODEL_FRAG, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/shadow.frag>
            ;
            shader_context->addData(SHADER_SHADOW_FRAG, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
        {
            static const char shader_string[] =
#include <shaders/shadow.vert>
            ;
            shader_context->addData(SHADER_SHADOW_VERT, new_t(BufferData, (void*)shader_string, sizeof(shader_string)));
        }
    }
    return shader_context;
}

const Shader::Property *Shader::attribute(const StringName &name) const {
    auto ite = attributes_index.find(name);
    if (ite != attributes_index.end()) return (const Property *)attributes[(long)(*ite).second];
    return NULL;
}

const Shader::Property *Shader::attribute(Type type, int index) const {
    int count = 0;
    for (auto ite = attributes.begin(); ite != attributes.end(); ++ite) {
        const Shader::Property *p = (const Shader::Property *)*ite;
        if (p->type == type) {
            if (count == index) return p;
            count ++;
        }
    }
    return NULL;
}

const Shader::Property *Shader::uniform(const StringName &name) const {
    auto ite = uniforms_index.find(name);
    if (ite != uniforms_index.end()) return (const Property *)uniforms[(long)(*ite).second];
    return NULL;
}

const Shader::Property *Shader::uniform(Type type, int index) const {
    int count = 0;
    for (auto ite = uniforms.begin(); ite != uniforms.end(); ++ite) {
        const Shader::Property *p = (const Shader::Property *)*ite;
        if (p->type == type) {
            if (count == index) return p;
            count ++;
        }
    }
    return NULL;
}

const Shader::Property *Shader::compileProperty(const gcore::StringName &name) const {
    auto ite = compiles_index.find(name);
    if (ite != compiles_index.end()) return (const Property *)compiles[(long)(*ite).second];
    return NULL;
}

void Shader::initialize(const StringName &vertex, const StringName &fragment) {
    process(vertex, fragment);
}

void Shader::process(const StringName &vertex, const StringName &fragment) {
    clear();
    variant_map m;
    ShaderContext::Result res;
    context()->parse(vertex, res);
    for (auto it = res.attributes.begin(), _e = res.attributes.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *item = (ShaderParser::AttributeItem*)*it;
        addAttribute(Shader::Property((Shader::Type)getVariantType(item->var_type),
                                          getVariantCount(item->var_type), item->name));
    }
    for (auto it = res.uniforms.begin(), _e = res.uniforms.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *item = (ShaderParser::AttributeItem*)*it;
        addUniform(Shader::Property((Shader::Type)getVariantType(item->var_type),
                                        getVariantCount(item->var_type), item->name));
    }
    for (auto it = res.compiles.begin(), _e = res.compiles.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *item = (ShaderParser::AttributeItem*)*it;
        addCompile(Shader::Property((Shader::Type)getVariantType(item->var_type),
                                    getVariantCount(item->var_type), item->name));
    }
    
    context()->parse(fragment, res);
    for (auto it = res.attributes.begin(), _e = res.attributes.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *item = (ShaderParser::AttributeItem*)*it;
        addAttribute(Shader::Property((Shader::Type)getVariantType(item->var_type),
                                      getVariantCount(item->var_type), item->name));
    }
    for (auto it = res.uniforms.begin(), _e = res.uniforms.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *item = (ShaderParser::AttributeItem*)*it;
        addUniform(Shader::Property((Shader::Type)getVariantType(item->var_type),
                                    getVariantCount(item->var_type), item->name));
    }
    for (auto it = res.compiles.begin(), _e = res.compiles.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *item = (ShaderParser::AttributeItem*)*it;
        addCompile(Shader::Property((Shader::Type)getVariantType(item->var_type),
                                    getVariantCount(item->var_type), item->name));
    }
    vert_name = vertex;
    frag_name = fragment;
}

void Shader::compile(const variant_map &compile_argvs) {
    treated_vert.clear();
    context()->compile(vert_name, compile_argvs, treated_vert);
    treated_frag.clear();
    context()->compile(frag_name, compile_argvs, treated_frag);
}

void Shader::clear() {
    for (auto it = attributes.begin(), _e = attributes.end(); it != _e; ++it) {
        delete (Shader::Property*)*it;
    }
    attributes.clear();
    attributes_index.clear();
    for (auto it = uniforms.begin(), _e = uniforms.end(); it != _e; ++it) {
        delete (Shader::Property*)*it;
    }
    uniforms.clear();
    uniforms_index.clear();
}

void Shader::addAttribute(const Shader::Property &property) {
    auto it = attributes_index.find(property.name);
    if (it == attributes_index.end()) {
        attributes_index[property.name] = (void*)attributes.size();
        attributes.push_back(new Shader::Property(property.type, property.size, property.name));
    }
}

void Shader::addUniform(const Shader::Property &property) {
    auto it = uniforms_index.find(property.name);
    if (it == uniforms_index.end()) {
        uniforms_index[property.name] = (void*)uniforms.size();
        uniforms.push_back(new Shader::Property(property.type, property.size, property.name));
    }
}

void Shader::addCompile(const Shader::Property &property) {
    auto it = compiles_index.find(property.name);
    if (it == compiles_index.end()) {
        compiles_index[property.name] = (void*)compiles.size();
        compiles.push_back(new Shader::Property(property.type, property.size, property.name));
    }
}

const string &Shader::getTreatedVertex() const {
    return treated_vert;
}

const string &Shader::getTreatedFragment() const {
    return treated_frag;
}
