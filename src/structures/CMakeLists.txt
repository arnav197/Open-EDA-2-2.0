cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(structures)
add_executable(OpenEDA_structures.cpp)



enable_testing()
add_test( NAME structures_test COMMAND  $<TARGET_FILE:OpenEDA_structures>)