#include <math.h>
#include <array>

#include "picosystem.hpp"

using namespace picosystem;

voice_t blip;
buffer_t *colour_picker = buffer(68, 68);

// a place to store our current selected colour along with slider attributes
struct slider_t {
  std::string name;
  uint32_t v;
  color_t c;
};

std::array<slider_t, 3> sliders = {{
  {"R", 15, rgb(15,  0,  0)},
  {"G",  6, rgb( 0, 15,  0)},
  {"B",  9, rgb( 0,  0, 15)}
}};

// selected colour coordinates
int32_t sx = 32, sy = 32;

void colour_from_xy(int32_t x, int32_t y, uint32_t &r, uint32_t &g, uint32_t &b) {
  // convert an x, y coordinate (0..63, 0..63) into an r, g, b triplet
  r = (x / 16) + (y / 16) * 4;
  g = y % 16;
  b = x % 16;
}

void prepare_rgb_palette() {
  uint32_t r, g, b;

  target(colour_picker);
  blend(COPY);

  // clear
  pen(0, 0, 0);
  clear();

  // draw outline
  pen(8, 8, 8);
  rect(0, 0, 68, 68);

  // draw the full palette grid of 64 x 64 pixels, this covers every single
  // colour in the picosystem 4096 colour (4 bits per channel) palette.
  for(int py = 0; py < 64; py++) {
    for(int px = 0; px < 64; px++) {
      colour_from_xy(px, py, r, g, b);
      pen(r, g, b);
      pixel(px + 2, py + 2);
    }
  }

  blend(ALPHA);
  target(SCREEN);
}

void init() {
  // a brief chirp for when user input is processed
  blip = voice(10, 0, 80, 0, 0, 0, 0, 100);
  prepare_rgb_palette();
}

void update(uint32_t tick) {
  if(!(tick % 5)) {
    // every 5th tick (every 50ms) check for user input and move/clamp the
    // cursor position accordingly
    if(button(   UP)) { sy--; sy = sy <  0 ?  0 : sy; play(blip, 1800, 10, 100); }
    if(button( DOWN)) { sy++; sy = sy > 63 ? 63 : sy; play(blip, 1800, 10, 100); }
    if(button( LEFT)) { sx--; sx = sx <  0 ?  0 : sx; play(blip, 1800, 10, 100); }
    if(button(RIGHT)) { sx++; sx = sx > 63 ? 63 : sx; play(blip, 1800, 10, 100); }

    // update our selected colour from the new cursor position
    colour_from_xy(sx, sy, sliders[0].v, sliders[1].v, sliders[2].v);
  }
}

void draw_rgb_palette(int32_t x, int32_t y) {
  int32_t cx, cy;

  blit(colour_picker, 0, 0, 69, 69, x, y);

  // calculate a brightness for the cursor that pulses over time
  uint8_t cursor_pulse = (sin(time() / 100.0f) + 1.0f) * 7.5f;
  pen(cursor_pulse, cursor_pulse, cursor_pulse);

  // draw cursor
  hline(sx - 5 + x + 2, sy + y + 2, 3);
  hline(sx + 2 + x + 2, sy + y + 2, 3);
  vline(sx + x + 2, sy - 5 + y + 2, 3);
  vline(sx + x + 2, sy + 2 + y + 2, 3);
}


void draw_slider(slider_t &slider, int32_t x, int32_t y) {
  static uint32_t w = 10, h = 68;

  // draw outline rectangle
  pen(slider.c);
  rect(x, y, w, h);

  // draw proportional filled value rectangle
  uint32_t sh = ((h - 4) * slider.v) / 15;
  frect(x + 2, y + h - sh - 2, w - 4, sh);
}

void draw(uint32_t tick) {
  // clear the screen
  pen(1, 1, 1);
  clear();

  // draw title
  // draw titleZZZZ
  // draw title
  pen(15, 15, 15);
  frect(0, 0, 120, 11);
  pen(1, 1, 1);
  text("Palette Explorer", 2, 2);

  // draw full palette
  draw_rgb_palette(5, 18);

  // draw r, g, b value sliders
  draw_slider(sliders[0], 80, 18);
  draw_slider(sliders[1], 92, 18);
  draw_slider(sliders[2], 104, 18);

  // draw selected colour swatch
  pen(8, 8, 8);
  rect(80, 92, 34, 23);
  color_t col = rgb(sliders[0].v, sliders[1].v, sliders[2].v);
  pen(col);
  frect(82, 94, 30, 19);

  // draw pen() call and constant value
  pen(13, 13, 13);

  std::string pen_call =
    "pen(" +
    str(sliders[0].v) + ", " +
    str(sliders[1].v) + ", " +
    str(sliders[2].v) + ")";

  text(pen_call, 5, 92);

  char buf[10];
  sprintf(buf, "%04x", col);
  std::string constant =
    "col = 0x" + std::string(buf);
  text(constant, 5, 107);
}