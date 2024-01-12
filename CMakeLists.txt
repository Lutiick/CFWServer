cmake_minimum_required(VERSION 3.22)
project(CFWServer C)

set(CMAKE_C_STANDARD 11)

find_library(libconfig libconfig)

set(SOURCE_FILES
        core/server.c
        utils/utils.c
        utils/utils.h
        core/server.h
        core/config.c
        core/config.h
        core/main.c)

add_executable(CFWServer ${SOURCE_FILES})
target_link_libraries(CFWServer config)
