//
// Created by gen on 16/5/31.
//

#ifndef HIRENDER_SHADER_H
#define HIRENDER_SHADER_H

#include <vector>
#include <IMP.h>
#include <core/Action.h>
#include "ShaderContext.h"
#include <render_define.h>

using namespace hicore;

namespace hirender {
    class Material;
    class ShaderIMP;

    CLASS_BEGIN_0_N(Shader)

    public:
        enum Type {
            Vecter2,
            Vecter3,
            Vecter4,

            Void,
            Int,
            Long,
            Float,
            Double,

            Matrix2,
            Matrix3,
            Matrix4,
            Sampler,
            
        };

        struct Property {
            Type type;
            int size;
            StringName name;

            _FORCE_INLINE_ Property(Type type, int size, const StringName &name):
                    type(type), name(name), size(size) {}
            
            _FORCE_INLINE_ bool operator==(const Property &other) const {
                return type == other.type && name == other.name && size == other.size;
            }
        };

    private:
        ShaderIMP           *imp;
        friend class Material;

        pointer_vector      attributes;
        pointer_map         attributes_index;
        pointer_vector      uniforms;
        pointer_map         uniforms_index;
        pointer_vector      compiles;
        pointer_map         compiles_index;
    
        static mutex        mtx;
        static Ref<Shader>  __default_shader;
        static Ref<Shader>  __ui_shader;
        static Ref<Shader>  __model_shader;
    
        static Ref<ShaderContext> shader_context;
    
        void clear();
        void addAttribute(const Property &property);
        void addUniform(const Property &property);
        void addCompile(const Property &property);
    
        StringName vert_name;
        StringName frag_name;
        string treated_vert;
        string treated_frag;
    
        void process(const StringName &vertex, const StringName &fragment);

    public:
    
        static const StringName SHADER_TEXT_VERT;
        static const StringName SHADER_TEXT_FRAG;
        static const StringName SHADER_UI_FRAG;
        static const StringName SHADER_SIMPLE_VERT;
        static const StringName SHADER_SIMPLE_FRAG;
        static const StringName SHADER_IMAGE_FRAG;
        static const StringName SHADER_MODEL_VERT;
        static const StringName SHADER_MODEL_FRAG;
        static const StringName SHADER_SHADOW_FRAG;
        static const StringName SHADER_SHADOW_VERT;
    
        _FORCE_INLINE_ const pointer_vector &getAttributes() const {
            return attributes;
        }
    
        _FORCE_INLINE_ const pointer_vector &getUniforms() const {
            return uniforms;
        }
    
        _FORCE_INLINE_ const pointer_vector &getCompiles() const {
            return compiles;
        }

        const Property *attribute(const StringName &name) const;
        const Property *attribute(Type type, int index) const;

        const Property *uniform(const StringName &name) const;
        const Property *uniform(Type type, int index) const;
    
        const Property *compileProperty(const StringName &name) const;
    
        const string &getTreatedVertex() const;
        const string &getTreatedFragment() const;
        _FORCE_INLINE_ ShaderIMP *getIMP() const {return imp;}

        _FORCE_INLINE_ Shader(const StringName &vertex, const StringName &fragment) : Shader(){
            initialize(vertex, fragment);
        }
        Shader();
        virtual void initialize(const StringName &vertex, const StringName &fragment);
        ~Shader();
    
        static const Ref<Shader> &defaultShader();
        static const Ref<Shader> &uiShader();
        static const Ref<Shader> &modelShader();
    
        static const Ref<ShaderContext> &context();
    
        void compile(const variant_map &compile_argvs);
    
    CLASS_END


    CLASS_BEGIN_TNV(ShaderIMP, Imp, 1, Shader)

    protected:
        enum PropertyType {
            Attribute,
            Uniform
        };
    
        const char *getShader(int type) {
            if (type) {
                return getTarget()->getTreatedFragment().c_str();
            }else
                return getTarget()->getTreatedVertex().c_str();
        }
    
        friend class Shader;

    CLASS_END
}


#endif //HIRENDER_SHADER_H
