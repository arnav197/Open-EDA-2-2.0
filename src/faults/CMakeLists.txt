cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(faults)

include(GoogleTest)
enable_testing()


add_executable(faults_tests.cpp faults_test.cpp)