//
// Created by Gen2 on 2018-12-19.
//

#ifndef GRENDER_TEST_TESTOBJECT_H
#define GRENDER_TEST_TESTOBJECT_H


#include <core/Object.h>
#include <core/Ref.h>
#include <core/Callback.h>

CLASS_BEGIN(TestObject, gc::RefObject)

private:
    int int_value;
    gc::RCallback callback;

public:

    METHOD _FORCE_INLINE_ int getIntValue() {
        return int_value;
    }

    METHOD _FORCE_INLINE_ void setIntValue(int value) {
        int_value = value;
    }

    PROPERTY(int_value, getIntValue, setIntValue)

    METHOD _FORCE_INLINE_ gc::RCallback getCallback() {
        return callback;
    }

    METHOD _FORCE_INLINE_ void setCallback(const gc::RCallback &callback) {
        this->callback = callback;
    }
    PROPERTY(callback, getCallback, setCallback);

protected:
    ON_LOADED_BEGIN(cls, gc::RefObject)
        ADD_PROPERTY(cls, "int_value", ADD_METHOD(cls, TestObject, getIntValue), ADD_METHOD(cls, TestObject, setIntValue));
        ADD_PROPERTY(cls, "callback", ADD_METHOD(cls, TestObject, getCallback), ADD_METHOD(cls, TestObject, setCallback));
    ON_LOADED_END
CLASS_END


#endif //GRENDER_TEST_TESTOBJECT_H
