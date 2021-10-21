set(MOD_NAME picosystem)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/picosystem.c
    ${CMAKE_CURRENT_LIST_DIR}/picosystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/voice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/state.cpp
    ${CMAKE_CURRENT_LIST_DIR}/primitives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/text.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utility.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hardware.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stats.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/picosystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/audio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/blend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/state.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/primitives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/text.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/utility.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/hardware.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/assets.cpp
)

pico_generate_pio_header(usermod_picosystem ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/screen.pio)
pico_generate_pio_header(usermod_picosystem ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/screen_double.pio)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_${MOD_NAME_UPPER}_ENABLED=1
    PIXEL_DOUBLE=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/picosystem.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers -Wno-implicit-int"
)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/picosystem.cpp
    PROPERTIES COMPILE_FLAGS
    "-DPIXEL_DOUBLE=1"
)

# TODO fix sign compare issues in codebase
set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/primitives.cpp
    PROPERTIES COMPILE_FLAGS
    "-Wno-error=sign-compare"
)
set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/text.cpp
    PROPERTIES COMPILE_FLAGS
    "-Wno-error=sign-compare"
)