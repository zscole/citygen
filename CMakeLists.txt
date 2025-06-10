
cmake_minimum_required(VERSION 3.10)
project(citygen C)

set(CMAKE_C_STANDARD 99)

include_directories(include)
file(GLOB SOURCES "src/*.c")

add_executable(citygen ${SOURCES} examples/main.c)
