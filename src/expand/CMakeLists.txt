cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(expand)

include(GoogleTest)
enable_testing()


add_executable(expand_tests.cpp expand_test.cpp)