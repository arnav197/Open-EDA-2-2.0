cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(simulation)

enable_testing()





add_test( NAME simulation_test COMMAND  simulation_tests)