cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(sat)

include(GoogleTest)
enable_testing()


add_executable(sat_tests.cpp sat_test.cpp)