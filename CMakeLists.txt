cmake_minimum_required(VERSION 3.5)

project(grender)

include_directories("." "thirdparties")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CORE_SRC
        core/script/Script.cpp
        core/math/Type.cpp
        core/IMP.cpp
        )

add_library(gcore STATIC ${CORE_SRC})

project(grender_test)

link_libraries(gcore)

add_definitions(-DUSING_SCRIPT)

include_directories(".")

set(TEST_SRC test/main.cpp test/TestObject.cpp test/TestObject.h)

add_executable(grender_test ${TEST_SRC})

