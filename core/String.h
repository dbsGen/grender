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
        std::string content;

    public:
        _FORCE_INLINE_ _String() {}
        _FORCE_INLINE_ _String(const char *content) {
            if (content) this->content = content;
        }
        _FORCE_INLINE_ _String(std::string content) {
            this->content = content;
        }
        _FORCE_INLINE_ _String(const _String &other) {
            this->content = other.content;
        }
        _FORCE_INLINE_ operator std::string() {
            return content;
        }
        _FORCE_INLINE_ operator const std::string() const {
            return content;
        }
        _FORCE_INLINE_ virtual std::string str() const {
            return content;
        }
        _FORCE_INLINE_ _String &operator=(const std::string &str) {
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

    class RString : public Ref<_String> {

    public:
        _FORCE_INLINE_ RString() {}

        _FORCE_INLINE_ RString(const char *content) : Ref(new _String(content)) {
        }

        _FORCE_INLINE_ RString(std::string content) : Ref(new _String(content)) {
        }

        _FORCE_INLINE_ RString(const Reference &ref) : Ref(ref) {
        }

        _FORCE_INLINE_ operator std::string() {
            return operator*()->str();
        }

        _FORCE_INLINE_ operator const std::string() const {
            return operator*()->str();
        }

        _FORCE_INLINE_ std::string str() const {
            return operator*()->str();
        }

        _FORCE_INLINE_ RString &operator=(const std::string &str) {
            operator*()->operator=(str);
            return *this;
        }

        _FORCE_INLINE_ RString &operator=(const _String &other) {
            operator*()->operator=(other);
            return *this;
        }

    };
}


#endif //HI_RENDER_PROJECT_ANDROID_STRING_H
