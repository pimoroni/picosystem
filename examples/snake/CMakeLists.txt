add_definitions(-DPIXEL_DOUBLE -DNO_STARTUP_LOGO)

add_executable(
  snake
  snake.cpp
)

# Pull in pico libraries that we need
target_link_libraries(snake picosystem)

# create map/bin/hex file etc.
pico_add_extra_outputs(snake)