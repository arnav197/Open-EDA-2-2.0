cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(window)
add_executable(OpenEDA_window.cpp)



enable_testing()
add_test( NAME window_test COMMAND  $<TARGET_FILE:OpenEDA_window>)