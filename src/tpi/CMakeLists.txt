cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(tpi)

include(GoogleTest)
enable_testing()


add_executable(tpi_tests.cpp tpi_test.cpp)