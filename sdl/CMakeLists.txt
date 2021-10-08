cmake_minimum_required(VERSION 3.12)

set(PICOSYSTEM_LIBRARY_PATH ${CMAKE_CURRENT_LIST_DIR}/../libraries/)
set(PICOSYSTEM_SDL 1)

# Compiler definitions we want to inherit from user target
set(allowed_defs "PIXEL_DOUBLE;NO_STARTUP_LOGO")

project(PicoSystemSDL)

add_subdirectory(${USER_CODE} ${CMAKE_CURRENT_BINARY_DIR}/out)

get_property(user_targets DIRECTORY ${USER_CODE} PROPERTY BUILDSYSTEM_TARGETS)

foreach(target ${user_targets})
    get_target_property(user_compile_definitions ${target} COMPILE_DEFINITIONS)
    foreach(def ${user_compile_definitions})
        if(${def} IN_LIST allowed_defs)
            target_compile_definitions(picosystem PRIVATE "-D${def}")
        endif()
    endforeach()
endforeach()
