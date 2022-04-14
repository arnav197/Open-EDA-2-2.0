cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(atpg)
enable_testing()




add_test( NAME atpg_test COMMAND  atpg_tests)