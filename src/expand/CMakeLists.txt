cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(expand)
enable_testing()





add_test( NAME expand_test COMMAND  expand_tests)