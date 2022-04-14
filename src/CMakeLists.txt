cmake_minimum_required(VERSION 3.13)

project(OpenEDA-2.0)

set(CMAKE_CXX_STANDARD 14)


include(CTest)

enable_testing()


add_subdirectory(parser)
add_subdirectory(atpg)
add_subdirectory(cop)
add_subdirectory(expand)
add_subdirectory(faults)
add_subdirectory(prpg)
add_subdirectory(sat)
add_subdirectory(simulation)
add_subdirectory(structures)
add_subdirectory(tpi)
add_subdirectory(trace)
add_subdirectory(window)