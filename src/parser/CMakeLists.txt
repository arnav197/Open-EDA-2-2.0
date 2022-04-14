cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)


project(parser)
add_executable(OpenEDA_parser.cpp)



enable_testing()
add_test( NAME parser_test COMMAND  $<TARGET_FILE:OpenEDA_parser>)