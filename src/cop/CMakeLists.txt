cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(cop)
add_executable(OpenEDA_cop.cpp)



enable_testing()
add_test( NAME cop_test COMMAND  $<TARGET_FILE:OpenEDA_cop>)