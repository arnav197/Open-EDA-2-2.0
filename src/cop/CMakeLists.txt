cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(cop)

include(GoogleTest)
enable_testing()


add_executable(cop_tests.cpp cop_test.cpp)