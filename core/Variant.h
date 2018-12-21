//
// Created by gen on 16/5/30.
//

#ifndef HICORE_VARIANT_H
#define HICORE_VARIANT_H

#include <cstdlib>
#include <string>
#include "Define.h"
#include "Reference.h"
#include "StringName.h"

#include "core_define.h"

namespace gcore {

    template<class T>
    class Ref;

    class Variant {
        BASE_FINAL_CLASS_DEFINE
    public:

        ENUM_BEGIN(Type)
            TypeNull = 0,
            TypeBool,
            TypeChar,
            TypeShort,
            TypeInt,
            TypeLong,
            TypeLongLong,
            TypeFloat,
            TypeDouble,
            TypePointer,
            TypeObject,
            TypeStringName,
            TypeReference,
            TypeMemory
        ENUM_END

        typedef union {
            bool    v_bool;
            char    v_char;
            short   v_short;
            int     v_int;
            long    v_long;
            long    v_longlong;
            float   v_float;
            double  v_double;
            void*   v_pointer;
        } u_value;

    private:

        u_value value;
        int8_t type;
        const Class *class_type;

        void release();
        void retain(const u_value &value, const Class *class_type, int8_t type = -1);
        static bool isRef(int8_t type);

        
        template<class T = Object>
        _FORCE_INLINE_ T *_get() const {
            return value.v_pointer;
        }
        
        static const Variant _null;

    public:

        _FORCE_INLINE_ static const Variant &null() {return _null;}

        _FORCE_INLINE_ Variant(void) : type(TypeNull) {
            value.v_pointer = NULL;
        }

        Variant(const Variant &other) : Variant() {
            value.v_pointer = other.value.v_pointer;
            retain(value, other.class_type, other.type);
        }

        
        _FORCE_INLINE_ Variant(const Reference &referene) : Variant() {
            retain(u_value{v_pointer:referene.get()}, referene.getType(), TypeReference);
        }

        _FORCE_INLINE_ ~Variant() {
            release();
        }
        
        template<class T = Object>
        _FORCE_INLINE_ T *get() const {
            return (T*)operator void *();
        }

//        void operator=(const HObject *object);
        void operator=(const Variant &other) {
            retain(other.value, other.class_type, other.type);
        }
        void operator=(Variant &&other) {
            release();
            retain(other.value, other.class_type, other.type);
        }

        bool operator==(const Variant &other) const;

        _FORCE_INLINE_ Object *operator->() const {
            return get();
        }

        const Class *getType() const;

        _FORCE_INLINE_ Reference ref() const {
            return isRef() ? Reference((Object*)value.v_pointer) : Reference();
        }

        _FORCE_INLINE_ bool isRef() const { return isRef(type); }
        _FORCE_INLINE_ bool empty() const { return type == TypeNull || (type >= TypePointer && value.v_pointer == NULL); }

        template <typename T>
        static Variant var(const T *target) {
            Variant v;
            v.retain(u_value{v_pointer: (void*)target}, T::getClass());
            return v;
        }

        operator bool() const;
        operator char() const;
        operator short() const;
        operator int() const;
        operator long() const;
        operator long long() const;
        operator float() const;
        operator double() const;

        operator std::string() const;
        
        operator void *() const;
        operator const char *() const;
        operator Object *() const;
        operator StringName() const;

        std::string str() const;
        
        Variant(char);
        Variant(short);
        Variant(int);
        Variant(long);
        Variant(long long);
        Variant(float);
        Variant(double);
        Variant(bool);
        Variant(const std::string &);
        Variant(const char *);
        Variant(const Object*);
        Variant(void*);
        Variant(const StringName &name);

        template <class T>
        static Variant make(T v) {
            return Variant(&v);
        }

    };

}


#endif //HICORE_VARIANT_H
