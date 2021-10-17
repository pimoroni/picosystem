#include <math.h>

#include "picosystem.hpp"

using namespace picosystem;

// initialise the world
void init() {
}

// process user input and update the world state
void update(uint32_t tick) {
}

void draw_number(int32_t v, int32_t x, int32_t y) {
  for(int i = 0; i < 32; i++) {
    text(v & (1 << (31 - i)) ? '1' : '0', x + (i * 7), y);
  }
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


/*
  pen(15, 15, 15);
  int i;

  i = 0x0f0f0f0f;
  draw_number(i, 5, 5);
  i >>= 1;
  i &= 0x0f0f0f0f;
  draw_number(i, 5, 15);

  i = 0x0f0f0f0f;
  draw_number(i, 5, 35);
  i /= 15;
  i &= 0x0f0f0f0f;
  draw_number(i, 5, 45);
*/

  start = time_us();
  blit(*_ss, 16, 16, 32, 32, 120 - (scale / 2) + xoff, 120 - (scale / 2) + yoff, scale, scale);
  pen(15, 15, 15);
  blend(COPY);
  text(str(time_us() - start), 50, 10);
  blend(BLEND);
  start = time_us();
  blit(*_ss, 16, 16, 32, 32, 120 - (scale / 2) + xoff, 120 - (scale / 2) - yoff);
  pen(15, 15, 15);
  blend(COPY);
  text(str(time_us() - start), 80, 10);
  blend(BLEND);
  pen(15, 2, 2, 2);
  start = time_us();
  frect(120 - (scale / 2) - xoff, 120 - (scale / 2) - yoff, scale, scale);
  pen(15, 15, 15);
  blend(COPY);
  text(str(time_us() - start), 150, 10);
  blend(BLEND);

  pen(15, 2, 2);
  start = time_us();
  scale = 40;
  frect(120 - (scale / 2) - xoff, 120 - (scale / 2) - yoff, scale, scale);
  pen(15, 15, 15);


  pen(15, 15, 15);
  rect(120 - (scale / 2) - xoff, 120 - (scale / 2) - yoff, scale, scale);


}


