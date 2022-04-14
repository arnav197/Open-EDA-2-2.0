cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(structures)
enable_testing()





add_test( NAME structures_test COMMAND structures_tests)