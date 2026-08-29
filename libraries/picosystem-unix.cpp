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