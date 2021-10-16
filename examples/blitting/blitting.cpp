#include <math.h>

#include "picosystem.hpp"

using namespace picosystem;

// initialise the world
void init() {
}

// process user input and update the world state
void update(uint32_t tick) {
}

// draw the world
void draw() {
  pen(1, 2, 3);
  clear();

  blit(*_ss, 16, 16, 32, 32, 10, 10);

  static uint32_t i = 0;
  i++;

  uint32_t scale = (sin(i / 10.0f) + 1.0f) * 200.0f;

  blit(*_ss, 16, 16, 32, 32, 10, 100, scale, scale);
}


