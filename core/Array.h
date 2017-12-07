//
//  Vector.h
//  hirender_iOS
//
//  Created by Gen on 16/9/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef Vector_h
#define Vector_h


#include "Object.h"
#include <vector>
#include <string>
#include "Ref.h"
#include "Variant.h"
#include "core_define.h"

using namespace std;

namespace hicore {
    class Array;
    
    CLASS_BEGIN_N(_Array, RefObject)
    private:
        variant_vector variants;
    friend class Array;

    public:
        _FORCE_INLINE_ _Array() {}
        _FORCE_INLINE_ _Array(const variant_vector &variants) {
            this->variants = variants;
        }
        virtual string str() const;
        _FORCE_INLINE_ _Array &operator=(const variant_vector &vs) {
            variants = vs;
            return *this;
        }
        _FORCE_INLINE_ _Array &operator=(const _Array &other) {
            variants = other.variants;
            return *this;
        }
        _FORCE_INLINE_ operator variant_vector() {
            return variants;
        }
        _FORCE_INLINE_ Variant &operator[](long n) {
            if (n >= variants.size()) variants.resize(n);
            return variants[n];
        }

        _FORCE_INLINE_ const Variant &operator[](long n) const {
            return n < variants.size() ? variants.at(n) : Variant::null();
        }
        _FORCE_INLINE_ size_t size() const {
            return variants.size();
        }
        void contact(const variant_vector &other) {
            for (auto it = other.begin(), _e = other.end(); it != _e; ++it) {
                variants.push_back(*it);
            }
        }
        _FORCE_INLINE_ variant_vector::iterator begin() {
            return variants.begin();
        }
        _FORCE_INLINE_ variant_vector::iterator end() {
            return variants.end();
        }
        _FORCE_INLINE_ void push_back(const Variant &var) {
            variants.push_back(var);
        }
        _FORCE_INLINE_ const Variant &at(long n) const {
            return n < variants.size() ? variants.at(n) : Variant::null();
        }

    CLASS_END

    CLASS_BEGIN_TN(Array, Ref, 1, _Array)

    public:
        _FORCE_INLINE_ Array() : Ref(new _Array(variant_vector())) {}
        _FORCE_INLINE_ Array(const variant_vector &variants) : Ref(new _Array(variants)) {
        }

        _FORCE_INLINE_ Array(const Reference &ref) : Ref(ref) {
        }

        _FORCE_INLINE_ Variant &at(long n) {
            return operator*()->operator[](n);
        }


        _FORCE_INLINE_ const Variant &at(long n) const {
            return operator*()->operator[](n);
        }

        _FORCE_INLINE_ size_t size() const {
            return operator*() ? operator*()->size() : 0;
        }
        variant_vector *vec() const {
            return &this->operator*()->variants;
        }

        _FORCE_INLINE_ operator Variant() const {
            return Variant(*this);
        }
        _FORCE_INLINE_ Array(const Variant &var) : Array() {
            if (var && var.getType()->isTypeOf(_Array::getClass())) {
                operator=(var.ref());
            }
        }
        _FORCE_INLINE_ void contact(const Array &other) {
            if (other) {
                operator*()->contact((*other)->variants);
            }
        }

    CLASS_END
}


#endif /* Vector_h */
