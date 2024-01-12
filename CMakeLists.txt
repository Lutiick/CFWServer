cmake_minimum_required(VERSION 3.22)
project(CFWServer C)

set(CMAKE_C_STANDARD 11)

add_executable(CFWServer core/main.c utils/utils.c utils/utils.h core/setup_server.cpp)
