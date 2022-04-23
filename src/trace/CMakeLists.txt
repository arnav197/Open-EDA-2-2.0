cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(trace)

include(GoogleTest)
enable_testing()


add_executable(trace_tests.cpp trace_test.cpp)