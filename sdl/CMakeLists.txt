cmake_minimum_required(VERSION 3.12)

set(PICOSYSTEM_LIBRARY_PATH ${CMAKE_CURRENT_LIST_DIR}/../libraries/)
set(PICOSYSTEM_SDL 1)

project(PicoSystemSDL)

add_subdirectory(${USER_CODE} ${CMAKE_CURRENT_BINARY_DIR}/out)