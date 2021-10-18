#include <stdio.h>
#include <cstdlib>

#include <math.h>

#include "picosystem.hpp"

namespace picosystem {

  uint32_t _debug;

  color_t _pen;
  uint8_t _a = 15;

  int32_t _tx = 0, _ty = 0;
  int32_t _camx = 0, _camy = 0;
  uint32_t _io = 0, _lio = 0;
  blend_func_t _bf = ALPHA;

  #ifdef PIXEL_DOUBLE
    color_t _fb[120 * 120];
    buffer_t *SCREEN = buffer(120, 120, _fb);
    int32_t _cx = 0, _cy = 0, _cw = 120, _ch = 120;
  #else
    color_t _fb[240 * 240];
    buffer_t *SCREEN = buffer(240, 240, _fb);
    int32_t _cx = 0, _cy = 0, _cw = 240, _ch = 240;
  #endif

  buffer_t *_dt = SCREEN;

  #ifdef NO_SPRITESHEET
    buffer_t *_ss = nullptr;
  #else
    buffer_t *SPRITESHEET = buffer(128, 128, (void *)_default_sprite_sheet);
    buffer_t *_ss = SPRITESHEET;
  #endif

  #ifdef NO_FONT
    uint8_t *_font = nullptr;
  #else
    uint8_t *_font = (uint8_t *)&_default_font[0][0];
  #endif

}

using namespace picosystem;

// main entry point - the users' code will be automatically
// called when they implement the init(), update(), and render()
// functions in their project

int main() {
  _init_hardware();

  // setup lut for fast sin/cos functions
  for(uint32_t i = 0; i < 256; i++) {
    _fsin_lut[i] = sin((_PI * 2.0f) * (float(i) / 256.0f));
  }

  #ifndef NO_STARTUP_LOGO
    // fade in logo by ramping up backlight
    pen(0, 0, 0); clear();
    pen(15, 15, 15); _logo();
    for(int i = 0; i < 75; i++) {
      backlight(i);
      _wait_vsync();
      _flip();
    }

    sleep(300); // ...and breathe out...

    // fade out logo in 16 colour steps
    for(int i = 15; i >= 0; i--) {
      pen(0, 0, 0); clear();
      pen(i, i, i); _logo();
      _wait_vsync();
      _flip();

      sleep(20);
    }
  #else
    backlight(75);
  #endif

  sleep(300);

  pen(0, 0, 0); clear();

  // call users init() function so they can perform any needed
  // setup for world state etc
  init();

  uint32_t update_rate_ms = 10;
  uint32_t pending_update_ms = 0;
  uint32_t last_ms = time();

  uint32_t tick = 0;

  _io = _gpio_get();

  while(true) {
    uint32_t ms = time();

    // work out how many milliseconds of updates we're waiting
    // to process and then call the users update() function as
    // many times as needed to catch up
    pending_update_ms += (ms - last_ms);
    while(pending_update_ms >= update_rate_ms) {
      _lio = _io;
      _io = _gpio_get();

      update(tick++);
      pending_update_ms -= update_rate_ms;
    }

    // if current flipping the framebuffer in the background
    // then wait until that is complete before allow the user
    // to render
    while(_is_flipping()) {}

    // call user render function to draw world
    draw();

    // wait for the screen to vsync before triggering flip
    // to ensure no tearing
    _wait_vsync();

    // flip the framebuffer to the screen
    _flip();

    last_ms = ms;
  }

}