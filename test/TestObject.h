//
// Created by Gen2 on 2018-12-19.
//

#ifndef GRENDER_TEST_TESTOBJECT_H
#define GRENDER_TEST_TESTOBJECT_H


#include <core/Object.h>
#include <core/Ref.h>

CLASS_BEGIN(TestObject, gcore::RefObject)

private:
    int int_value;

public:

    METHOD _FORCE_INLINE_ int getIntValue() {
        return int_value;
    }

    METHOD _FORCE_INLINE_ void setIntValue(int value) {
        int_value = value;
    }

    PROPERTY(int_value, getIntValue, setIntValue)

protected:
    ON_LOADED_BEGIN(cls, gcore::RefObject)
        ADD_PROPERTY(cls, "int_value", ADD_METHOD(cls, TestObject, getIntValue), ADD_METHOD(cls, TestObject, setIntValue));
    ON_LOADED_END
CLASS_END


#endif //GRENDER_TEST_TESTOBJECT_H
