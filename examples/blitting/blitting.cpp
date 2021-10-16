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

  //uint32_t scale = (sin(i / 10.0f) + 1.0f) * 200.0f;

  uint32_t scale = 150;
  uint32_t start = time_us();

  int32_t xoff = sin(i / 20.0f) * 100;
  int32_t yoff = cos(i / 20.0f) * 100;
  blit(*_ss, 16, 16, 32, 32, 120 - (scale / 2) + xoff, 120 - (scale / 2) + yoff, scale, scale);

  // blit(*_ss, 16, 16, 32, 32, 180, 50, scale, scale);
  // blit(*_ss, 16, 16, 32, 32, 180, 180, scale, scale);


  // blit(*_ss, 16, 16, 32, 32, -50, 100, scale, scale);


  pen(15, 15, 15);
  text(str(time_us() - start), 50, 10);

  //start = time_us();
  //frect(120, 50, scale, scale);
  //text(str(time_us() - start), 150, 10);
}


