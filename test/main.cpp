#include <stdio.h>
#include "TestObject.h"
#include <core/String.h>
#include <core/Array.h>
#include <vector>
#include <core/Callback.h>

#include <gtest/gtest.h>

using namespace gcore;

#define STR(S) #S

TEST(Class, ClassRelationships)
{
    EXPECT_EQ(TestObject::getClass()->getParent(), RefObject::getClass());
    EXPECT_TRUE(TestObject::getClass()->isTypeOf(RefObject::getClass()));
    EXPECT_TRUE(TestObject::getClass()->isSubclassOf(RefObject::getClass()));
    EXPECT_FALSE(TestObject::getClass()->isSubclassOf(TestObject::getClass()));
}

TEST(Object, NameOfTestObject)
{
    Ref<TestObject> obj(new TestObject());
    EXPECT_STREQ(obj->getClass()->getFullname().str(), STR(TestObject));
}

TEST(Object, SetterAndGetter)
{
    Ref<TestObject> obj(new TestObject());
    obj->callArgs("setIntValue", 1023);
    EXPECT_EQ((int)obj->call("getIntValue"), obj->getIntValue());
}

int main(int argc, char* argv[]) {
    Ref<TestObject> obj(new TestObject());

    printf("ClassName %s\n", obj->getClass()->getFullname().str());

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

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}