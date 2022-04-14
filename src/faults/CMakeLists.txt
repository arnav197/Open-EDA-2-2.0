cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(faults)
add_executable(OpenEDA_faults.cpp)



enable_testing()
add_test( NAME faults_test COMMAND  $<TARGET_FILE:OpenEDA_faults>)