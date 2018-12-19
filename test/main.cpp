#include <stdio.h>
#include "TestObject.h"

int main() {
    gcore::Ref<TestObject> obj(new TestObject());

    printf("ClassName %s\n", obj->getClass()->getName());

    gcore::Variant iv = 1023;
    obj->call("setIntValue", pointer_vector{&iv});
    printf("int value is %d\n", (int)obj->call("getIntValue"));
    return 0;
}