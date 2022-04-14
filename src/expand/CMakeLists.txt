cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(expand)
add_executable(OpenEDA_expand.cpp)



enable_testing()
add_test( NAME expand_test COMMAND  $<TARGET_FILE:OpenEDA_expand>)