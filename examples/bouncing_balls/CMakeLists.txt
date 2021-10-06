add_executable(
  bouncing_balls
  bouncing_balls.cpp
)

# Pull in pico libraries that we need
target_link_libraries(bouncing_balls picosystem)

# create map/bin/hex file etc.
pico_add_extra_outputs(bouncing_balls)