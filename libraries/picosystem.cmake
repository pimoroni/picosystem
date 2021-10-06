add_library(picosystem INTERFACE)

pico_generate_pio_header(picosystem ${CMAKE_CURRENT_LIST_DIR}/screen.pio)
pico_generate_pio_header(picosystem ${CMAKE_CURRENT_LIST_DIR}/screen_double.pio)

target_sources(picosystem INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/picosystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/blend.cpp
  ${CMAKE_CURRENT_LIST_DIR}/state.cpp
  ${CMAKE_CURRENT_LIST_DIR}/primitives.cpp
  ${CMAKE_CURRENT_LIST_DIR}/utility.cpp
  ${CMAKE_CURRENT_LIST_DIR}/hardware.cpp
  ${CMAKE_CURRENT_LIST_DIR}/assets.cpp
)

target_include_directories(picosystem INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(picosystem INTERFACE pico_stdlib hardware_pio hardware_spi hardware_pwm hardware_dma hardware_irq hardware_adc hardware_interp)
