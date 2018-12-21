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


namespace gcore {
    
    CLASS_BEGIN_N(Callback, RefObject)
    
    public:
        Callback() {}
    
        template <typename ...Args>
        _FORCE_INLINE_ Variant operator() (Args && ...args) {
            return invoke(variant_vector{{args...}});
        }
        METHOD virtual Variant invoke(const RArray &params);
        EVENT(Variant, _invoke, const RArray &params);
    
    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_METHOD(cls, Callback, invoke);
        ON_LOADED_END
    CLASS_END
    
    // The Function is too big, do not command to use.
    template <int N>
    _FORCE_INLINE_ static const Variant &_var(const RArray &params) {
        return N < params.size() ? params[N] : Variant::null();
    }
    
    template <class M>
    class Function{};
    
    template<class ..._ArgType>
    class Function<void(_ArgType...)> : public Callback {

        std::function<void(_ArgType...)> func;
        
    public:
        _FORCE_INLINE_ Function() {}
        _FORCE_INLINE_ Function(const Function &other) : func(other.func) {
        }
        _FORCE_INLINE_ Function(std::function<void(_ArgType...)> func) : func(func) {
        }
        template <typename M>
        _FORCE_INLINE_ Function(M func) : func(func) {
        }
        
        _FORCE_INLINE_ Function &operator=(const Function &other) {
            this->func = other.func;
            return *this;
        }
        _FORCE_INLINE_ bool operator==(const Function &other) {
            return func == other.func;
        }
        
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const RArray &params, seq<Is...>*) const {
            func(type_convert<typename std::tuple_element<Is, std::tuple<_ArgType...> >::type >::toType(_var<Is>(params))...);
            return Variant::null();
        }
        
        virtual Variant invoke(const RArray &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            return _call(params, &d);
        }
        
    };
    
    template<class _Rp, class ..._ArgType>
    class Function<_Rp(_ArgType...)> : public Callback {

        std::function<_Rp(_ArgType...)> func;
        
    public:
        _FORCE_INLINE_ Function() {}
        _FORCE_INLINE_ Function(const Function &other) : func(other.func) {
        }
        _FORCE_INLINE_ Function(std::function<_Rp(_ArgType...)> func) : func(func) {
        }
        template <typename M>
        _FORCE_INLINE_ Function(M func) : func(func) {
        }
        
        _FORCE_INLINE_ Function &operator=(const Function &other) {
            this->func = other.func;
            return *this;
        }
        _FORCE_INLINE_ bool operator==(const Function &other) {
            return func == other.func;
        }
        
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const RArray &params, seq<Is...>*) const {
            return func(type_convert<typename std::tuple_element<Is, std::tuple<_ArgType...> >::type >::toType(_var<Is>(params))...);
        }
        
        virtual Variant invoke(const RArray &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            return _call(params, &d);
        }
    };
    
    template <class M>
    class SFunction{};
    template<class ..._ArgType>
    class SFunction<void(*)(_ArgType...)> : public Callback {
        void (*func)(_ArgType...);
        Variant data;
        
    public:
        
        _FORCE_INLINE_ SFunction() {}
        _FORCE_INLINE_ SFunction(const SFunction &other) : func(other.func), data(other.data) {
        }
        
        _FORCE_INLINE_ SFunction(void(*func)(_ArgType...)) : func(func) {
        }
        _FORCE_INLINE_ SFunction(void(*func)(_ArgType...), const Variant &data) : func(func), data(data) {
        }
        
        _FORCE_INLINE_ SFunction &operator=(const SFunction &other) {
            func = other.func;
            data = other.data;
            return *this;
        }
        _FORCE_INLINE_ bool operator==(const SFunction &other) {
            return func == other.func;
        }
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const RArray &params, seq<Is...>*) const {
            func(type_convert<typename std::tuple_element<Is, std::tuple<_ArgType...> >::type >::toType(_var<Is>(params))...);
            return Variant::null();
        }
        virtual Variant invoke(const RArray &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            if (data) {
                RArray arr;
                arr->push_back(data);
                arr.contact(params);
                return _call(std::move(arr), &d);
            }else {
                return _call(params, &d);
            }
        }
    };
    
    template<class _Rp, class ..._ArgType>
    class SFunction<_Rp(*)(_ArgType...)> : public Callback {
        _Rp (*func)(_ArgType...);
        Variant data;
    public:
        
        _FORCE_INLINE_ SFunction() {}
        _FORCE_INLINE_ SFunction(const SFunction &other) : func(other.func), data(other.data) {
        }
        
        _FORCE_INLINE_ SFunction(_Rp(*func)(_ArgType...)) : func(func) {
        }
        _FORCE_INLINE_ SFunction(_Rp(*func)(_ArgType...), const Variant &data) : func(func), data(data) {
        }
        _FORCE_INLINE_ SFunction &operator=(const SFunction &other) {
            func = other.func;
            data = other.data;
            return *this;
        }
        _FORCE_INLINE_ bool operator==(const SFunction &other) {
            return func == other.func;
        }
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const RArray &params, seq<Is...>*) const {
            return func(type_convert<typename std::tuple_element<Is, std::tuple<_ArgType...> >::type >::toType(_var<Is>(params))...);
        }
        virtual Variant invoke(const RArray &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            if (data) {
                RArray arr;
                arr.push_back(data);
                arr.contact(params);
                return _call(std::move(arr), &d);
            }else {
                return _call(params, &d);
            }
        }
    };
    
    template <class M>
    class MFunction{};
    template<class TC, class ..._ArgType>
    class MFunction<void(TC::*)(_ArgType...)> : public Callback {
        void (TC::*func)(_ArgType...);
        TC *target;
        Variant data;
        
    public:
        
        _FORCE_INLINE_ MFunction() {}
        _FORCE_INLINE_ MFunction(const MFunction &other) : func(other.func), target(other.target), data(other.data) {
        }
        
        _FORCE_INLINE_ MFunction(TC *target,void(TC::*func)(_ArgType...)) : func(func), target(target) {
        }
        _FORCE_INLINE_ MFunction(TC *target,void(TC::*func)(_ArgType...), const Variant &data) : func(func), target(target), data(data) {
        }
        
        _FORCE_INLINE_ MFunction &operator=(const MFunction &other) {
            target = other.target;
            func = other.func;
            return *this;
        }
        _FORCE_INLINE_ bool operator==(const MFunction &other) {
            return target == other.target && func == other.func;
        }
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const RArray &params, seq<Is...>*) const {
            (target->*func)(type_convert<typename std::tuple_element<Is, std::tuple<_ArgType...> >::type >::toType(_var<Is>(params))...);
            return Variant::null();
        }
        virtual Variant invoke(const RArray &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            if (!data.empty()) {
                RArray arr;
                arr.push_back(data);
                arr.contact(params);
                return _call(std::move(arr), &d);
            }else {
                return _call(params, &d);
            }
        }
    };
    
    template<class TC, class _Rp, class ..._ArgType>
    class MFunction<_Rp(TC::*)(_ArgType...)> : public Callback {
        _Rp (TC::*func)(_ArgType...);
        TC *target;
        Variant data;
    public:
        
        _FORCE_INLINE_ MFunction() {}
        _FORCE_INLINE_ MFunction(const MFunction &other) : func(other.func), target(other.target), data(data) {
        }
        
        _FORCE_INLINE_ MFunction(TC *target,_Rp(TC::*func)(_ArgType...)) : func(func), target(target) {
        }
        _FORCE_INLINE_ MFunction(TC *target,_Rp(TC::*func)(_ArgType...), const Variant &data) : func(func), target(target), data(data) {
        }
        _FORCE_INLINE_ MFunction &operator=(const MFunction &other) {
            target = other.target;
            func = other.func;
            return *this;
        }
        _FORCE_INLINE_ bool operator==(const MFunction &other) {
            return target == other.target && func == other.func;
        }
        template <int... Is>
        _FORCE_INLINE_ Variant _call(const RArray &params, seq<Is...>*) const {
            return (target->*func)(type_convert<typename std::tuple_element<Is, std::tuple<_ArgType...> >::type >::toType(_var<Is>(params))...);
        }
        virtual Variant invoke(const RArray &params) {
            static const int size = sizeof...(_ArgType);
            gen_seq<size> d;
            if (data) {
                RArray arr;
                arr->push_back(data);
                arr.contact(params);
                return _call(std::move(arr), &d);
            }else {
                return _call(params, &d);
            }
        }
    };
    
    template <typename T>
    struct ft : public ft<decltype(&T::operator())>
    {};
    // For generic types, directly use the result of the signature of its 'operator()'
    
    template <typename ClassType, typename ReturnType, typename... Args>
    struct ft<ReturnType(ClassType::*)(Args...) const>
    // we specialize for pointers to member function
    {
        
//        typedef ...Args args;
        typedef ReturnType(function_type)(Args...);
        
    };

    class RCallback : public Ref<Callback> {
    
public:
    template <typename ...Args>
    Variant operator() (Args && ...args) const {
        if (this->operator bool())
            return (*this->operator*())(args...);
        else {
            return Variant::null();
        }
    }
    _FORCE_INLINE_ RCallback() {}
    _FORCE_INLINE_ RCallback(Callback *ref) : Ref(ref) {
    }
    template <class T>
    _FORCE_INLINE_ RCallback(Function<T> *ref) : Ref(ref) {
    }
    template <class T>
    _FORCE_INLINE_ RCallback(SFunction<T> *ref) : Ref(ref) {
    }
    _FORCE_INLINE_ RCallback(const Reference &ref) : Ref(ref) {
    }
    _FORCE_INLINE_ RCallback(const Variant &var) : Ref(var) {
    }
    _FORCE_INLINE_ RCallback(const RCallback &ref) : Ref(ref) {
    }

    };
    
    template <typename M>
    RCallback C(M f) {
        typedef ft<M> func;
        return new Function<typename func::function_type>(f);
    }
    template <class _Rp, class ..._ArgType>
    RCallback C(_Rp(*f)(_ArgType...)) {
        return new SFunction<_Rp(*)(_ArgType...)>(f);
    }
    template <class _Rp, class ..._ArgType>
    RCallback C(_Rp(*f)(_ArgType...), Variant data) {
        return new SFunction<_Rp(*)(_ArgType...)>(f, data);
    }
    template <class T, class _Rp1, class _C2, class ..._ArgType>
    RCallback C(T t, _Rp1(_C2::*f)(_ArgType...)) {
        return new MFunction<_Rp1(_C2::*)(_ArgType...)>(t, f);
    }
    template <class T, class _Rp1, class _C2, class ..._ArgType>
    RCallback C(T t, _Rp1(_C2::*f)(_ArgType...), Variant data) {
        return new MFunction<_Rp1(_C2::*)(_ArgType...)>(t, f, data);
    }
}


#endif /* Callback_h */
