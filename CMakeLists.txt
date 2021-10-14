cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)
include (pico_extras_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Would find_package kindly search PATHS
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# Find the PicoSystem library
find_package(PICOSYSTEM CONFIG REQUIRED PATHS ..)

add_subdirectory(examples)