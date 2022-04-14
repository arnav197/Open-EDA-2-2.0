cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(trace)
add_executable(OpenEDA_trace.cpp)



enable_testing()
add_test( NAME trace_test COMMAND  $<TARGET_FILE:OpenEDA_trace>)