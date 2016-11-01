cmake_minimum_required(VERSION 2.7.8)

project(cpuinfo)

if(CMAKE_VERSION VERSION_LESS "2.8.12")
    function(add_compile_options)
        add_definitions(${ARGN})
    endfunction(add_compile_options)
endif()

add_compile_options(-std=c99 -Wall -Wextra -Wno-unused-parameter -Wconversion -Wshadow -pedantic)
add_definitions(-DNDEBUG)

add_library(cpuinfo src/cpuinfo.c)

TARGET_LINK_LIBRARIES(cpuinfo ${CARET_QT5_LINK})

