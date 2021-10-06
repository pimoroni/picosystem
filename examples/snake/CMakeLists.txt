cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(../../pico_sdk_import.cmake)

project(snake C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(PICOSYSTEM REQUIRED)

picosystem_executable(
  snake
  snake.cpp
)

pixel_double(snake)
disable_startup_logo(snake)