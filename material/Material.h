//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_MATERIAL_H
#define HIRENDER_MATERIAL_H

#include "Shader.h"
#include <IMP.h>
#include "../math/Type.h"
#include <texture/Texture.h>
#include <render_define.h>

namespace gr {
    class Renderer;
    class MaterialIMP;
    class Mesh;

    CLASS_BEGIN_N(Material, gc::RefObject)
    public:
        enum _CullFace {
            Front   = 0,
            Back,
            Both,
            None
        };
        typedef int CullFace;
    
    private:
        bool depth;
        bool blend;
        CullFace    cull_face;
        MaterialIMP *imp;
        friend class Renderer;
        friend class Mesh;

        gc::Ref<Shader>     shader;

        const Shader::Property *position_attribute;
        const Shader::Property *projection_uniform;
        const Shader::Property *view_uniform;
        const Shader::Property *translate_uniform;

        bool        compile_changed;
        variant_map uniform_values;
        variant_map compile_values;
        Matrix4     projection;
        Matrix4     view;
        int         render_index;
        static gc::Ref<Material> color_material;
        static gc::Ref<Material> view_material;

    protected:
        _FORCE_INLINE_ virtual void preProcess() {}
        virtual void _copy(const Object *other);

    public:
        static const gc::StringName DEFAULT_PROJECTION;
        static const gc::StringName DEFAULT_TRANSLATE;
        static const gc::StringName DEFAULT_TEXTURE;
        static const gc::StringName DEFAULT_POSITION;
        static const gc::StringName DEFAULT_VIEW;
        static const gc::StringName DEFAULT_COLOR;
        static const gc::StringName DEFAULT_UV;

        MaterialIMP *getIMP() {return imp;}

        void process();
        _FORCE_INLINE_ bool getDepth() {return depth;}
        _FORCE_INLINE_ void setDepth(bool d) {depth = d;}

        _FORCE_INLINE_ bool getBlend() {return blend;}
        _FORCE_INLINE_ void setBlend(bool b) {blend = b;}
    
        _FORCE_INLINE_ CullFace getCullFace() {return cull_face;}
        _FORCE_INLINE_ void setCullFace(CullFace cull_face) {this->cull_face = cull_face;}

        static const gc::Ref<Material> &colorMaterial();
        static const gc::Ref<Material> &viewMaterial();
        Material();

        INITIALIZE(Material, const gc::Ref<Shader> &shader,
                   setShader(shader);
        );
        ~Material();

        const gc::Ref<Shader> &getShader();
        void setShader(const gc::Ref<Shader> &shader);

        _FORCE_INLINE_ const variant_map &getUniformValues() {return uniform_values;}
        void setUniform(const gc::StringName &name, const gc::Variant &obj);
        void setUniform(const Shader::Property *property, const gc::Variant &obj);
        _FORCE_INLINE_ gc::Variant *getUniform(const gc::StringName &name) {
            auto ite = uniform_values.find((void*)name);
            return ite == uniform_values.end() ? NULL : &(*ite).second;
        }
        _FORCE_INLINE_ const gc::Variant *getUniform(const gc::StringName &name) const {return getUniform(name);}

        void setCompileValue(const gc::StringName &name, const gc::Variant &var);
    
    // Nessory?
//        template <class T>
//        _FORCE_INLINE_ T *getUniform(const StringName &name) {
//            Variant *v = getUniform(name);
//            return (v && (v->getType()->isTypeOf(T::getClass()))) ? v->get<T>() : NULL;
//        }
        template <class T>
        _FORCE_INLINE_ gc::Ref<T> getUniformRef(const gc::StringName &name) {
            gc::Variant *v = getUniform(name);

            if (v->isRef() && v->getTypeClass()->isTypeOf(T::getClass())) {
                return v->ref();
            }
            return NULL;
        }

        const Shader::Property *getPositionAttribute() {return position_attribute;}
        const Shader::Property *getProjectionUniform() {return projection_uniform;}
        const Shader::Property *getViewUniform() {return view_uniform;}
        const Shader::Property *getTranslateUniform() {return translate_uniform;}
        void setPositionAttribute(const Shader::Property *pa);
        void setProjectionUniform(const Shader::Property *pu);
        void setViewUniform(const Shader::Property *vu);
        void setTranslateUniform(const Shader::Property *tu);

        void setProjection(const Matrix4 &p);
        void setView(const Matrix4 &v);
        _FORCE_INLINE_ const Matrix4 &getProjection() {
            return projection;
        }
        _FORCE_INLINE_ const Matrix4 &getView() {
            return view;
        }

        void setTexture(const gc::Ref<Texture> &tex, int index);
        const gc::Ref<Texture> getTexture(int index);

        _FORCE_INLINE_ int getRenderIndex() {
            return render_index;
        }
        _FORCE_INLINE_ void setRenderIndex(int idx) {
            render_index = idx;
        }

    CLASS_END

    CLASS_BEGIN_TNV(MaterialIMP, Imp, 1, Material)

    protected:
        virtual void cleanPosition() = 0;
        virtual void cleanTranslate() = 0;

        virtual void updateShader(const Shader *shader) = 0;

        virtual void process() = 0;
        friend class Material;

    CLASS_END
};


#endif //HIRENDER_MATERIAL_H
