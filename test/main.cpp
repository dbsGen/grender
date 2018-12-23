#include <stdio.h>
#include "TestObject.h"
#include <core/String.h>
#include <core/Array.h>
#include <vector>
#include <core/Callback.h>
#include <script/ruby/RubyScript.h>

#include <gtest/gtest.h>

using namespace gcore;
using namespace gscript;

#define PATH "/Users/gen2/Programs/grender/test/ruby"


TEST(Ruby, RunSimpleScript)
{
    RubyScript ruby;
    EXPECT_EQ((int)ruby.runScript("1+2"), 3);
    RArray arr = ruby.runScript("[1,'hello', 3]");

    EXPECT_EQ((int)arr[0], 1);
    EXPECT_STREQ((const char *)arr[1], "hello");
    EXPECT_EQ((int)arr[2], 3);
}

TEST(Ruby, RunEnvFile)
{
    Ref<TestObject> obj;

    RubyScript ruby;
    ruby.setup(PATH);

    ruby.run(PATH "/test.rb");

    obj = ruby.runScript("$to");

    EXPECT_EQ(obj->getIntValue(), 333);

    obj->setCallback(C([](const std::string &str){
        EXPECT_STREQ(str.c_str(), "InRuby");
    }));
    ruby.runScript("$to.call_cb");
}

int main(int argc, char* argv[]) {
    TestObject::getClass();

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}