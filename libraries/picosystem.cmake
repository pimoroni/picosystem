add_library(picosystem INTERFACE)

pico_generate_pio_header(picosystem ${CMAKE_CURRENT_LIST_DIR}/screen.pio)
pico_generate_pio_header(picosystem ${CMAKE_CURRENT_LIST_DIR}/screen_double.pio)

target_sources(picosystem INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/picosystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/blend.cpp
  ${CMAKE_CURRENT_LIST_DIR}/audio.cpp
  ${CMAKE_CURRENT_LIST_DIR}/state.cpp
  ${CMAKE_CURRENT_LIST_DIR}/primitives.cpp
  ${CMAKE_CURRENT_LIST_DIR}/utility.cpp
  ${CMAKE_CURRENT_LIST_DIR}/hardware.cpp
  ${CMAKE_CURRENT_LIST_DIR}/assets.cpp
)

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

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2 DESTINATION .)
endfunction()

function(picosystem_asset NAME PATH)
  get_filename_component(PATH ${PATH} ABSOLUTE)
  get_filename_component(ASSET ${PATH} NAME)
  get_filename_component(PATH ${PATH} DIRECTORY)
  set(OBJNAME ${ASSET}.o)
  message("Building ${OBJNAME}")
  add_custom_command(OUTPUT ${OBJNAME}
    WORKING_DIRECTORY ${PATH}
    COMMAND ${CMAKE_LINKER} -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME} ${ASSET})
  # TODO figure out how to make static resources work
    ## COMMAND ${CMAKE_OBJCOPY} --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME} ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME})

  target_sources(${NAME} PRIVATE ${OBJNAME})
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