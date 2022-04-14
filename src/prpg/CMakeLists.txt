cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(prpg)
add_executable(OpenEDA_prpg.cpp)



enable_testing()
add_test( NAME prpg_test COMMAND  $<TARGET_FILE:OpenEDA_prpg>)