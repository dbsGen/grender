#include <stdio.h>
#include "TestObject.h"
#include <core/String.h>
#include <core/Array.h>
#include <vector>
#include <core/Callback.h>

using namespace gcore;

int main() {
    Ref<TestObject> obj(new TestObject());

    printf("ClassName %s\n", obj->getClass()->getFullname().str());

//    Variant iv = 1023;
    obj->callArgs("setIntValue", 1023);
    printf("int value is %d -> %d\n", (int)obj->call("getIntValue"), obj->getIntValue());

    printf("Ref size %d\n", sizeof(Variant));

    RCallback cb = C([](int l){
        printf("output %d\n", l);
    });

    cb(30.32);
    cb(2883);
    cb("你好");

    RCallback cb2 = C([](Object *object){
        if (object) {
            printf("output %s\n", object->getInstanceClass()->getFullname().str());
        }else {
            printf("Object is NULL\n");
        }
    });

    cb2(30.32);
    cb2(2883);
    cb2("你好");
    cb2(obj);

    return 0;
}