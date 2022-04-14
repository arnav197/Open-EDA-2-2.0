cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(sat)
add_executable(OpenEDA_sat.cpp)



enable_testing()
add_test( NAME sat_test COMMAND  $<TARGET_FILE:OpenEDA_sat>)