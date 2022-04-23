cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(window)

include(GoogleTest)
enable_testing()


add_executable(window_tests.cpp window_test.cpp)