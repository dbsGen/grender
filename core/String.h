//
// Created by gen on 16/7/5.
//

#ifndef HI_RENDER_PROJECT_ANDROID_STRING_H
#define HI_RENDER_PROJECT_ANDROID_STRING_H

#include "Object.h"
#include <string>
#include "Ref.h"
#include "core_define.h"

namespace gcore {
    CLASS_BEGIN_N(_String, RefObject)
    private:
        string content;

    public:
        _FORCE_INLINE_ _String() {}
        _FORCE_INLINE_ _String(const char *content) {
            if (content) this->content = content;
        }
        _FORCE_INLINE_ _String(string content) {
            this->content = content;
        }
        _FORCE_INLINE_ _String(const _String &other) {
            this->content = other.content;
        }
        _FORCE_INLINE_ operator string() {
            return content;
        }
        _FORCE_INLINE_ operator const string() const {
            return content;
        }
        _FORCE_INLINE_ virtual string str() const {
            return content;
        }
        _FORCE_INLINE_ _String &operator=(const string &str) {
            content = str;
            return *this;
        }
        _FORCE_INLINE_ _String &operator=(const _String &other) {
            content = other.content;
            return *this;
        }
        _FORCE_INLINE_ const char *c_str() {
            return content.c_str();
        }

    CLASS_END

    CLASS_BEGIN_TN(String, Ref, 1, _String)

    public:
        _FORCE_INLINE_ String() {}
        _FORCE_INLINE_ String(const char *content) : Ref(new _String(content)) {
        }
        _FORCE_INLINE_ String(string content) : Ref(new _String(content)) {
        }
        _FORCE_INLINE_ String(const Reference &ref) : Ref(ref) {
        }
        _FORCE_INLINE_ operator string() {
            return operator*()->str();
        }
        _FORCE_INLINE_ operator const string() const {
            return operator*()->str();
        }
        _FORCE_INLINE_ virtual string str() const {
            return operator*()->str();
        }
        _FORCE_INLINE_ String &operator=(const string &str) {
            operator*()->operator=(str);
            return *this;
        }
        _FORCE_INLINE_ String &operator=(const _String &other) {
            operator*()->operator=(other);
            return *this;
        }

    CLASS_END
}


#endif //HI_RENDER_PROJECT_ANDROID_STRING_H
