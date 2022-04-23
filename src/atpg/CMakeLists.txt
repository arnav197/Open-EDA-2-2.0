cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(atpg)
include(GoogleTest)
enable_testing()


add_executable(atpg_tests.cpp atpg_test.cpp)





