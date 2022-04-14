cmake_minimum_required(VERSION 3.13)

project(OpenEDA-2.0)

set(CMAKE_CXX_STANDARD 14)

add_executable(OpenEDA_main.cpp)

include(CTest)

enable_testing()
add_test( NAME main_test COMMAND  $<TARGET_FILE:OpenEDA_main>)

add_subdirectory("src/parser")
add_subdirectory("src/atpg")
add_subdirectory("src/cop")
add_subdirectory("src/expand")
add_subdirectory("src/faults")
add_subdirectory("src/prpg")
add_subdirectory("src/sat")
add_subdirectory("src/simulation")
add_subdirectory("src/structures")
add_subdirectory("src/tpi")
add_subdirectory("src/trace")
add_subdirectory("src/window")