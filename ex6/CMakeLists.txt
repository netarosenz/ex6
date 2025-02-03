cmake_minimum_required(VERSION 3.29)
project(ex6 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -g")

include_directories(.)

add_executable(ex6
    ex6.c
    ex6.h)
