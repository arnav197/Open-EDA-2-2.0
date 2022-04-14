cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(cop)
enable_testing()




add_test( NAME cop_test COMMAND  cop_test.cpp)