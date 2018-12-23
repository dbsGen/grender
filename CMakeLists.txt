cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(gcore)

add_subdirectory(gcore)

project(grender_test)

link_directories("test/libs")
include_directories("test/include")

link_libraries(gcore gmock gtest)

add_definitions(-DUSING_SCRIPT)

include_directories(".")

set(TEST_SRC test/main.cpp test/TestObject.cpp test/TestObject.h)

add_executable(grender_test ${TEST_SRC})

