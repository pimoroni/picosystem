#include <stdio.h>
#include <cstdlib>

#include <math.h>

#include "picosystem.hpp"


namespace picosystem {

  stat_t stats;

  color_t _pen;
  uint8_t _a = 15;

  int32_t _tx = 0, _ty = 0;
  int32_t _tlh = 8, _tls = 1;
  int32_t _tlw = -1;

  int32_t _camx = 0, _camy = 0;
  uint32_t _io = 0, _lio = 0;
  blend_func_t _bf = ALPHA;

  #ifndef DYNAMIC_BUFFER
  #ifdef PIXEL_DOUBLE
    color_t _fb[120 * 120] __attribute__ ((aligned (4))) = { };
    buffer_t *SCREEN = buffer(120, 120, _fb);
    int32_t _cx = 0, _cy = 0, _cw = 120, _ch = 120;
  #else
    color_t _fb[240 * 240] __attribute__ ((aligned (4))) = { };
    buffer_t *SCREEN = buffer(240, 240, _fb);
    int32_t _cx = 0, _cy = 0, _cw = 240, _ch = 240;
  #endif
  #else
    buffer_t *SCREEN = nullptr;
    int32_t _cx = 0, _cy = 0, _cw = 120, _ch = 120;
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

#ifndef MICROPY_BUILD_TYPE
int main() {
  _init_hardware();
  _start_audio();

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
    // Keep the screen off...
    backlight(0);
    // Screen buffer is initialized clear; just flip it.
    _flip();
    // Wait for the DMA transfer to finish
    while (_is_flipping());
    // wait for the screen to update
    _wait_vsync();
    _wait_vsync();
    // Turn the screen on
    backlight(75);
  #endif

  // call users init() function so they can perform any needed
  // setup for world state etc
  init();

  uint32_t tick = 0;
  uint32_t last_frame_ms = 0;
  uint32_t start_flip = 0;

  _io = _gpio_get();

  while(true) {
    uint32_t start_tick_us = time_us();

    // reset drawing state except font and spritesheet
    pen();
    alpha();
    clip();
    blend();
    target();
    camera();
    cursor();

    // store previous io state and get new io state
    _lio = _io;
    _io = _gpio_get();

    // call users update() function
    uint32_t start_update_us = time_us();
    update(tick++);
    stats.update_us = time_us() - start_update_us;

    // if we're currently transferring the the framebuffer to the screen then
    // wait until that is complete before allowing the user to do their drawing
    uint32_t wait_us = 0;
    uint32_t start_wait_flip_us = time_us();
    while(_is_flipping()) {}
    stats.flip_us = time_us() - start_flip;
    wait_us += time_us() - start_wait_flip_us;

    // call user render function to draw world
    uint32_t start_draw_us = time_us();
    draw(tick);
    stats.draw_us = time_us() - start_draw_us;

    // wait for the screen to vsync before triggering flip
    // to ensure no tearing
    uint32_t start_wait_vsync_us = time_us();
    _wait_vsync();
    wait_us += time_us() - start_wait_vsync_us;

    // flip the framebuffer to the screen
    start_flip = time_us();
    _flip();

    tick++;

    stats.tick_us = time_us() - start_tick_us;

    // calculate fps and round to nearest value (instead of truncating/floor)
    stats.fps = (1000000 - 1) / stats.tick_us + 1;

    if(stats.fps > 40) {
      // if fps is high enough then we definitely didn't miss vsync
      stats.idle = (wait_us * 100) / stats.tick_us;
    }else{
      // if we missed vsync then we overran the frame time and hence had
      // no idle time
      stats.idle = 0;
    }
  }

}
#endif
