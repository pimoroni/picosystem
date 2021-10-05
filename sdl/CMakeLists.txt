cmake_minimum_required(VERSION 3.12)

set(PICOSYSTEM_LIBRARY_PATH ${CMAKE_CURRENT_LIST_DIR}/../libraries/)

project(PicoSystemSDL)

function (pico_add_extra_outputs)
endfunction()

if(NOT SDL2_LIBRARIES OR NOT SDL2_INCLUDE_DIRS)
    find_package(SDL2 REQUIRED)
endif()

# If SDL2 was built using CMake, the generated configuration files define SDL2::* targets instead of the SDL2_* variables
if(TARGET SDL2::SDL2)
    set(SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2)
# handle SDL2 built with only a static library
elseif(TARGET SDL2::SDL2-static)
    set(SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2-static)
else()
    target_include_directories(picosystem
        PRIVATE	${SDL2_INCLUDE_DIRS}
    )
endif()

add_library(picosystem
    ${CMAKE_CURRENT_LIST_DIR}/hardware.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/picosystem.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/blend.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/state.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/primitives.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/utility.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/assets.cpp
)

target_include_directories(picosystem PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../libraries/)

target_link_libraries(picosystem PUBLIC ${SDL2_LIBRARIES})

add_subdirectory(${USER_CODE} ${CMAKE_CURRENT_BINARY_DIR}/out)