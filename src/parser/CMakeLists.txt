cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(parser)

include(GoogleTest)
enable_testing()


add_executable(parser_tests.cpp parser_test.cpp)