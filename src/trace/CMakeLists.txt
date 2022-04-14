cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(trace)
enable_testing()






add_test( NAME trace_test COMMAND  trace_tests)