cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(atpg)
add_executable(OpenEDA_atpg.cpp)



enable_testing()
add_test( NAME atpg_test COMMAND  $<TARGET_FILE:OpenEDA_atpg>)