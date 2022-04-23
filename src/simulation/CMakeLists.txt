cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(simulation)

include(GoogleTest)
enable_testing()


add_executable(simulation_tests.cpp simulation_test.cpp)