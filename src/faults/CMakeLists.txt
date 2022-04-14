cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(faults)

enable_testing()



add_test( NAME faults_test COMMAND faults_tests)