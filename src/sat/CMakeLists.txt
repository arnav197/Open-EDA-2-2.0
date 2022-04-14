cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(sat)
enable_testing()





add_test( NAME sat_test COMMAND  sat_tests)