cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(prpg)

include(GoogleTest)
enable_testing()


add_executable(prpg_tests.cpp prpg_test.cpp)