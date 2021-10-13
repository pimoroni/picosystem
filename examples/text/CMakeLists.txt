cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(../../pico_sdk_import.cmake)

project(text C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

picosystem_executable(
  text
  text.cpp
)

pixel_double(text)
disable_startup_logo(text)