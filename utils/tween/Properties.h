//
// Created by gen on 16/9/15.
//

#ifndef VOIPPROJECT_PROPERTIES_H
#define VOIPPROJECT_PROPERTIES_H


#include <core/math/Type.h>
#include <core/math/Math.hpp>
#include <object/Object.h>
#include <object/ui/View.h>

#include "Tween.h"

namespace hirender {
    CLASS_BEGIN_NV(TweenPProperty, TweenProperty)

        Variant from, to;
        StringName property_name;
        const Property *property;

        friend class Tween;

    protected:
        const Property *getProperty(HObject *object);
        const Variant &getFrom(HObject *target);
        _FORCE_INLINE_ const Variant &getTo() const {
            return to;
        }
        _FORCE_INLINE_ void setFrom(const Variant &from) {
            this->from = from;
        }
        _FORCE_INLINE_ void setTo(const Variant &to) {
            this->to = to;
        }
        TweenPProperty(const StringName &property_name);

    CLASS_END

    template <class T, class V, typename SETTER, typename GETTER>
    CLASS_BEGIN_NV(TweenSProperty, TweenProperty)

    public:
        typedef V (*lerp_method)(const V &from, const V &to, float per);

    private:

        bool set_from;
        V from;
        V to;
        SETTER setter;
        GETTER getter;
        lerp_method lerp;

    public:
        TweenSProperty(SETTER setter, GETTER getter, V to) {
            set_from = false;
            this->to = to;
            this->setter = setter;
            this->getter = getter;
            V (*lm)(const V &from, const V &to, float p) = hicore::lerp;
            lerp = lm;
        }
        TweenSProperty(SETTER setter, GETTER getter, V from, V to) {
            set_from = true;
            this->from = from;
            this->to = to;
            this->setter = setter;
            this->getter = getter;
            V (*lm)(const V &from, const V &to, float p) = hicore::lerp;
            lerp = lm;
        }
        TweenSProperty(SETTER setter, GETTER getter, V from, V to, lerp_method lerp) {
            set_from = true;
            this->from = from;
            this->to = to;
            this->setter = setter;
            this->getter = getter;
            this->lerp = lerp;
        }

        virtual void process(HObject *target, double p) {
            if (!set_from) {
                from = (*target->cast_to<T>().*getter)();
                set_from = true;
            }
            (*target->cast_to<T>().*setter)(lerp(from, to, p));
        }

    CLASS_END

    template <class T, class V, typename SETTER, typename GETTER, typename TO_M>
    CLASS_BEGIN_NV(TweenDynamicProperty, TweenProperty)

    public:
        typedef V (*lerp_method)(const V &from, const V &to, float per);

    private:

        bool set_from;
        V from;
        TO_M to;
        SETTER setter;
        GETTER getter;
        lerp_method lerp;

    public:
        TweenDynamicProperty(SETTER setter, GETTER getter, TO_M to) {
            set_from = false;
            this->to = to;
            this->setter = setter;
            this->getter = getter;
            V (*lm)(const V &from, const V &to, float p) = hicore::lerp;
            lerp = lm;
        }
        TweenDynamicProperty(SETTER setter, GETTER getter, V from, TO_M to) {
            set_from = true;
            this->from = from;
            this->to = to;
            this->setter = setter;
            this->getter = getter;
            V (*lm)(const V &from, const V &to, float p) = hicore::lerp;
            lerp = lm;
        }
        TweenDynamicProperty(SETTER setter, GETTER getter, V from, TO_M to, lerp_method lerp) {
            set_from = true;
            this->from = from;
            this->to = to;
            this->setter = setter;
            this->getter = getter;
            this->lerp = lerp;
        }

        virtual void process(HObject *target, double p) {
            if (!set_from) {
                from = (*target->cast_to<T>().*getter)();
                set_from = true;
            }
            (*target->cast_to<T>().*setter)(lerp(from, to(p), p));
        }

    CLASS_END

    template <class T, class V, typename SETTER_T, typename GETTER_T>
    TweenProperty *makeProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)(), V to) {
        return new TweenSProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)()>(setter, getter, to);
    };
    template <class T, class V, typename SETTER_T, typename GETTER_T>
    TweenProperty *makeProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)() const, V to) {
        return new TweenSProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)() const>(setter, getter, to);
    };
    template <class T, class V, typename SETTER_T, typename GETTER_T>
    TweenProperty *makeProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)(), V from, V to) {
        return new TweenSProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)()>(setter, getter, from, to);
    };
    template <class T, class V, typename SETTER_T, typename GETTER_T>
    TweenProperty *makeProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)() const, V from, V to) {
        return new TweenSProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)() const>(setter, getter, from, to);
    };
    template <class T, class V, typename SETTER_T, typename GETTER_T, typename LERP>
    TweenProperty *makeProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)(), V from, V to, LERP lerp) {
        return new TweenSProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)()>(setter, getter, from, to, lerp);
    };
    template <class T, class V, typename SETTER_T, typename GETTER_T, typename LERP>
    TweenProperty *makeProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)() const, V from, V to, LERP lerp) {
        return new TweenSProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)() const>(setter, getter, from, to, lerp);
    };

    template <class T, class V, typename SETTER_T, typename GETTER_T, typename LERP, typename TO_M>
    TweenProperty *makeDynamicProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)(), V from, TO_M to, LERP lerp) {
        return new TweenDynamicProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)(), TO_M>(setter, getter, from, to, lerp);
    };
    template <class T, class V, typename SETTER_T, typename GETTER_T, typename LERP, typename TO_M>
    TweenProperty *makeDynamicProperty(void(T::*setter)(SETTER_T), GETTER_T(T::*getter)() const, V from, TO_M to, LERP lerp) {
        return new TweenDynamicProperty<T, V, void(T::*)(SETTER_T), GETTER_T(T::*)() const, TO_M>(setter, getter, from, to, lerp);
    };
    
    _FORCE_INLINE_ TweenProperty *poseProperty(const Matrix4 &from, const Matrix4 &to) {
        return makeProperty(&Object::setPose, &Object::getPose, from, to, &Matrix4::lerp);
    }
    _FORCE_INLINE_ TweenProperty *positionProperty(const Vector3f &from, const Vector3f &to) {
        return makeProperty(&Object::setPosition, &Object::getPosition, from, to);
    }
    _FORCE_INLINE_ TweenProperty *rotationProperty(const Vector4f &from, const Vector4f &to) {
        return makeProperty(&Object::setRotation, &Object::getRotation, from, to);
    }
    _FORCE_INLINE_ TweenProperty *scaleProperty(const Vector3f &from, const Vector3f &to) {
        return makeProperty(&Object::setScale, &Object::getScale, from, to);
    }
    _FORCE_INLINE_ TweenProperty *alphaProperty(float from, float to) {
        return makeProperty(&View::setAlpha, &View::getAlpha, from, to);
    }

    CLASS_BEGIN_N(Matrix4PProperty , TweenPProperty)

    public:
        _FORCE_INLINE_ Matrix4PProperty() : TweenPProperty("") {}
        Matrix4PProperty(const StringName &name, const Matrix4 &to);
        Matrix4PProperty(const StringName &name, const Matrix4 &from, const Matrix4 &to);

        virtual void process(HObject *target, double p);

    CLASS_END
}

#endif //VOIPPROJECT_PROPERTIES_H
