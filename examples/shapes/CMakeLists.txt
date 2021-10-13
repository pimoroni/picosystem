cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(../../pico_sdk_import.cmake)

project(shapes C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


picosystem_executable(
    shapes
    shapes.cpp
)

pixel_double(shapes)
disable_startup_logo(shapes)