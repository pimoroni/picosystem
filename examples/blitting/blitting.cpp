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
  blend(BLEND);
  pen(1, 2, 3);
  clear();

  blit(*_ss, 16, 16, 32, 32, 10, 10);

  static uint32_t i = 0;
  i++;

  uint32_t start;
  uint32_t scale = 150;
  uint32_t move = 20;
  int32_t xoff = sin(i / 20.0f) * move;
  int32_t yoff = cos(i / 20.0f) * move;




  start = time_us();
  blit(*_ss, 16, 16, 32, 32, 120 - (scale / 2) + xoff, 120 - (scale / 2) + yoff, scale, scale);
  pen(15, 15, 15);
  text(str(time_us() - start), 50, 10);

  pen(15, 2, 2, 2);
  start = time_us();
  frect(120 - (scale / 2) - xoff, 120 - (scale / 2) - yoff, scale, scale);
  pen(15, 15, 15);
  text(str(time_us() - start), 150, 10);

  pen(15, 2, 2);
  start = time_us();
  scale = 40;
  frect(120 - (scale / 2) - xoff, 120 - (scale / 2) - yoff, scale, scale);
  pen(15, 15, 15);
  text(str(time_us() - start), 150, 10);


  pen(15, 15, 15);
  rect(120 - (scale / 2) - xoff, 120 - (scale / 2) - yoff, scale, scale);

  // 300,000 cycles in 1,200us (which is how long a 150x150 blit takes with MASK)
  // 150x150 is 22,500 pixels so that's 13 cycles per pixel

}


