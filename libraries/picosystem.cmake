add_library(picosystem INTERFACE)

pico_generate_pio_header(picosystem ${CMAKE_CURRENT_LIST_DIR}/screen.pio)
pico_generate_pio_header(picosystem ${CMAKE_CURRENT_LIST_DIR}/screen_double.pio)

target_sources(picosystem INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/picosystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/blend.cpp
  ${CMAKE_CURRENT_LIST_DIR}/audio.cpp
  ${CMAKE_CURRENT_LIST_DIR}/state.cpp
  ${CMAKE_CURRENT_LIST_DIR}/primitives.cpp
  ${CMAKE_CURRENT_LIST_DIR}/text.cpp
  ${CMAKE_CURRENT_LIST_DIR}/utility.cpp
  ${CMAKE_CURRENT_LIST_DIR}/hardware.cpp
  ${CMAKE_CURRENT_LIST_DIR}/assets.cpp
)

set(PICOSYSTEM_LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/memmap_picosystem.ld)

target_include_directories(picosystem INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(picosystem INTERFACE pico_stdlib hardware_pio hardware_spi hardware_pwm hardware_dma hardware_irq hardware_adc hardware_interp)

function(picosystem_executable NAME SOURCES)
  add_executable(
    ${NAME}
    ${SOURCES}
    ${ARGN}
  )

  # Pull in pico libraries that we need
  target_link_libraries(${NAME} picosystem)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${NAME})

  pico_set_linker_script(${NAME} ${PICOSYSTEM_LINKER_SCRIPT})

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2 DESTINATION .)
endfunction()

function(pixel_double NAME)
  target_compile_options(${NAME} PRIVATE -DPIXEL_DOUBLE)
endfunction()

function(disable_startup_logo NAME)
  target_compile_options(${NAME} PRIVATE -DNO_STARTUP_LOGO)
endfunction()

function(no_font NAME)
  target_compile_options(${NAME} PRIVATE -DNO_FONT)
endfunction()

function(no_spritesheet NAME)
  target_compile_options(${NAME} PRIVATE -DNO_SPRITESHEET)
endfunction()

function(no_overclock NAME)
  target_compile_options(${NAME} PRIVATE -DNO_OVERCLOCK)
endfunction()