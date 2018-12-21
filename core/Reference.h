//
// Created by gen on 16/8/31.
//

#ifndef VOIPPROJECT_REFERENCE_H
#define VOIPPROJECT_REFERENCE_H

#include "Define.h"

namespace gcore {
    class Object;
    class Class;
    class Variant;
    class StringName;

    class Reference {
    BASE_FINAL_CLASS_DEFINE
    private:
        static Reference nullRef;

        Object *ptr;

    protected:
        void release();
        void retain();

        friend class ClassDB;

    public:
        Reference(const Reference &other) : ptr(other.ptr) {
            retain();
        }
        _FORCE_INLINE_ Reference() : ptr(NULL) {}
        template <typename T>
        _FORCE_INLINE_ Reference(T *p) : ptr(p) {retain();}

        _FORCE_INLINE_ ~Reference() {
            release();
        }

        void ref(const Reference *other);
        _FORCE_INLINE_ void clear() {
            release();
            ptr = NULL;
        }
        _FORCE_INLINE_ static const Reference &null() {
            return nullRef;
        }

        _FORCE_INLINE_ Reference &operator=(const Reference &other) {
            ref(&other);
            return *this;
        }
        Reference &operator=(Object *p);
        _FORCE_INLINE_ Object *operator->() {return ptr;}
        _FORCE_INLINE_ Object *operator->() const {return ptr;}

        _FORCE_INLINE_ Object &operator*() {return *ptr;}
        _FORCE_INLINE_ Object &operator*() const {return *ptr;}

        _FORCE_INLINE_ bool operator==(const Reference &other) const {
            return ptr == other.ptr;
        }
        template<class T>
        _FORCE_INLINE_ bool operator==(const T *other) const {
            return ptr == other;
        }
        _FORCE_INLINE_ bool operator!=(const Reference &other) const {
            return ptr != other.ptr;
        }
        _FORCE_INLINE_ operator Object*() const {
            return ptr;
        }
        _FORCE_INLINE_ Object* get() const {
            return ptr;
        }
        const Class *getType() const;
        _FORCE_INLINE_ operator bool() const {
            return ptr != NULL;
        }
        void call(const StringName &name, Variant *result, const Variant **params, int count);
        
        Reference(const Variant &other);

        std::string str() const;
    };
}

#endif //VOIPPROJECT_REFERENCE_H
