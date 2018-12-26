//
//  Callback.h
//  hirender_iOS
//
//  Created by Gen on 16/9/24.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef Callback_h
#define Callback_h

#include "Object.h"
#include "Reference.h"
#include "Variant.h"
#include "Array.h"
#include "MethodImp.h"
#include "core_define.h"

namespace gc {
    
    CLASS_BEGIN_N(Callback, RefObject)
    
    public:
        Callback() {}
    
        template <typename ...Args>
        _FORCE_INLINE_ Variant operator() (Args && ...args) {
            return invoke(variant_vector{{args...}});
        }
        METHOD virtual Variant invoke(const Array &params);
        EVENT(Variant, _invoke, const Array &params);
    
    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_METHOD(cls, Callback, invoke);
        ON_LOADED_END
    CLASS_END
    
    // The Function is too big, do not command to use.
    template <int N>
    _FORCE_INLINE_ static const Variant &_var(const Array &params) {
        return N < params.size() ? (*params)->operator[](N) : Variant::null();
    }
    
    template <class M>
    class Function{};
    
    template<class ..._ArgType>
    class Function<void(_ArgType...)> : public Callback {
        
        function<void(_ArgType...)> func;
        
    public:
        _FORCE_INLINE_ Function() {}
        _FORCE_INLINE_ Function(const Function &other) : func(other.func) {
        }
        _FORCE_INLINE_ Function(function<void(_ArgType...)> func) : func(func) {
        }
        template <typename M>
        _FORCE_INLINE_ Function(M func) : func(func) {
        }
        
        _FORCE_INLINE_ Function &operator=(const Function &other) {
            this->func = other.func;
        }
        
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const Array &params, seq<Is...>*) const {
            func((_var<Is>(params))...);
            return Variant::null();
        }
        
        _FORCE_INLINE_ virtual Variant invoke(const Array &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            return _call(params, &d);
        }
        
    };
    
    template<class _Rp, class ..._ArgType>
    class Function<_Rp(_ArgType...)> : public Callback {
        
        function<_Rp(_ArgType...)> func;
        
    public:
        _FORCE_INLINE_ Function() {}
        _FORCE_INLINE_ Function(const Function &other) : func(other.func) {
        }
        _FORCE_INLINE_ Function(function<_Rp(_ArgType...)> func) : func(func) {
        }
        template <typename M>
        _FORCE_INLINE_ Function(M func) : func(func) {
        }
        
        _FORCE_INLINE_ Function &operator=(const Function &other) {
            this->func = other.func;
        }
        
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const Array &params, seq<Is...>*) const {
            return func((_var<Is>(params))...);
        }
        
        _FORCE_INLINE_ virtual Variant invokev(const Array &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            return _call(params, &d);
        }
    };
}


#endif /* Callback_h */
