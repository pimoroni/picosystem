#include <math.h>
#include <array>
#include <algorithm>

#include "picosystem.hpp"

using namespace picosystem;

struct blend_mode_t {
  std::string name;
  blend_func_t bf;
};

std::array<blend_mode_t, 10> blend_modes = {{
  {"ALPHA", ALPHA},
  {"COPY", COPY},
  {"MASK", MASK},
  {"PEN", PEN},
  {"DARKEN", DARKEN},
  {"LIGHTEN", LIGHTEN},
  {"ADD", ADD},
  {"SUBTRACT", SUBTRACT},
  {"MULTIPLY", MULTIPLY},
  {"DISSOLVE", DISSOLVE},
}};

uint32_t blend_mode = 0;

// initialise the world
void init() {
}

// process user input and update the world state
void update(uint32_t tick) {
  if(pressed(UP)) {
    blend_mode = blend_mode == 0 ? blend_modes.size() - 1 : blend_mode - 1;
  }

  if(pressed(DOWN)) {
    blend_mode = blend_mode == blend_modes.size() - 1 ? 0 : blend_mode + 1;
  }
}

// draw the world
void draw(uint32_t tick) {
  blend_mode_t bm = blend_modes[blend_mode];

  blend(COPY);
  pen(0, 0, 0);
  clear();

  blend(ALPHA);
  alpha();
  blit(SPRITESHEET, 0, 0, 120, 120, 0, 0);

  int32_t x = 60 + sin(time() / 1000.0f) * 30;
  int32_t y = 60 + cos(time() / 1000.0f) * 30;
  int32_t v = (sin(time() / 1000.0f) + 1.0f) * 8.0f;
  blend(bm.bf);

  color_t p;
  if(bm.bf == ALPHA) {
    p = rgb(255, 0, 0, v);
  } else if (bm.bf == COPY) {
    p = rgb(0, 255, 0, v);
  } else if (bm.bf == MASK) {
    p = rgb(0, 0, 255, v);
  } else if (bm.bf == PEN) {
    p = rgb(0, 255, 255, v);
  } else if (bm.bf == DARKEN) {
    p = rgb(v, v, v);
  } else if (bm.bf == LIGHTEN) {
    p = rgb(v, v, v);
  } else if (bm.bf == ADD) {
    p = rgb(v, v, v);
  } else if (bm.bf == SUBTRACT) {
    p = rgb(v, v, v);
  } else if (bm.bf == MULTIPLY) {
    p = rgb(v, v, v);
  } else if (bm.bf == DISSOLVE) {
    p = rgb(255, 255, 0, v);
  }

  // draw overlay with pen and blend mode
  pen(p);
  frect(0, y, 120, 120 - y);

  // draw boundary
  blend(ALPHA);
  pen(15, 15, 15, 8);
  frect(0, y, 120, 2);

  // draw menu
  blend(ALPHA);
  pen(0, 0, 0);
  frect(0, 0, 60, 120);
  for(int i = 0; i < blend_modes.size(); i++) {
    pen(0, 0, 0);
    text(blend_modes[i].name, 2, i * 9 + 3);
    if(i == blend_mode) {
      pen(15, 15, 15);
    } else {
      pen(8, 8, 8);
    }
    text(blend_modes[i].name, 2, i * 9 + 2);
  }

  // draw swatch
  pen(p);
  blend(COPY);
  frect(2, 98, 20, 20);

  pen(15, 15, 15);
  text(str(v), 30, 100);

  text(str((v * 100) / 15) + "%", 30, 110);

}