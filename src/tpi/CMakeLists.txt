cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(tpi)
add_executable(OpenEDA_tpi.cpp)



enable_testing()
add_test( NAME tpi_test COMMAND  $<TARGET_FILE:OpenEDA_tpi>)