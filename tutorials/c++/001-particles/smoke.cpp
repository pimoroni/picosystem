#include <math.h>

#include "picosystem.hpp"
#include "include.hpp"

using namespace picosystem;

int32_t speed = 50;

v2_t repeller_pos;
int32_t repeller_strength = 50;

struct smoke_particle_t : public particle_t {
  v2_t drift;

  void create() {
    // select a starting position and randomise drift (spread/lift)
    pos = {rndf(50, 70), 120};
    drift = {rndf(-12.0f, 12.0f), rndf(-30.0f, -20.0f)};
  }

  void update(float elapsed) {
    // apply drift to particles, ensures smoke spreads and rises over time
    pos += (drift * (float(speed) / 100.0f)) * elapsed;

    // calculate distance to repeller
    v2_t repeller_vector = pos - repeller_pos;
    float distance = repeller_vector.length();

    // calculate strength of force of repeller based on distance
    float scaled_strength = 0.0f;
    if(distance < repeller_strength) {
      scaled_strength = (repeller_strength - distance) / repeller_strength;
    }

    // update position based on repeller vector and force
    repeller_vector.normalise();
    pos += repeller_vector * scaled_strength;
  }

  void draw() {
    // draw the particle
    fcircle(pos.x, pos.y, 4.0f);
  }
};

// create emitter (500 particles, 10 second lifetime)
emitter_t<smoke_particle_t> smoke(1000, 10000);

void init() {
}

// using the ALPHA blend mode means we can only increment all three channels
// at once, this limits the number of "steps" of smoke to just 8 different
// shades of grey.
//
// this custom blend function checks for which of the three colour channels is
// the smallest and only increments that one. this triples the number of "steps"
// giving a smoother and more nuanced result.
void SMOKE(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
  while(c--) {
    uint32_t dr = (*pd & 0x000f) >>  0;
    uint32_t dg = (*pd & 0xf000) >> 12;
    uint32_t db = (*pd & 0x0f00) >>  8;

    // select which channel to increment
    uint32_t add = 0;
    if(dr <= dg && dr <= db && dr < 8) {
      // increment red
      add = (1 <<  0);
    } else if(db <= dg && db < 8) {
      // increment green
      add = (1 <<  8);
    } else if(dg < 8) {
      // increment blue
      add = (1 << 12);
    }

    (*pd++) += add;
  }
}

// select our custom blender as the default
blend_func_t blender = SMOKE;

void update(uint32_t tick) {
  if(pressed(Y)) {
    smoke.reset();
  }

  if(pressed(B)) {
    blender = blender == ALPHA ? SMOKE : ALPHA;
  }

  if(button(RIGHT)) {
    repeller_strength = clamp(++repeller_strength, 1, 100);
  }

  if(button(LEFT)) {
    repeller_strength = clamp(--repeller_strength, 1, 100);
  }

  if(button(UP)) {
    speed = clamp(++speed, 1, 100);
  }

  if(button(DOWN)) {
    speed = clamp(--speed, 1, 100);
  }

  if(pressed(X)) {
    if(smoke.max_count > 250) {
      smoke.max_count -= 250;
    } else {
      smoke.max_count = 1000;
    }

    smoke.reset();
  }

  // move repeller to new location
  repeller_pos.x = (sin(time() / 800.0f) * 50.0f) + 60.0f;
  repeller_pos.y = (cos(time() / 1500.0f) * 50.0f) + 60.0f;

  // update particles
  smoke.update();
}

void draw(uint32_t tick) {
  // clear the screen
  pen(0, 0, 0);
  clear();

  // choose selected blend mode and draw particles
  blend(blender);
  pen(8, 8, 8, 1);
  smoke.draw();

  // reset blend mode
  blend();

  // draw repeller
  pen(4, 15, 8, 8);
  fcircle(repeller_pos.x, repeller_pos.y, 2);
  fcircle(repeller_pos.x, repeller_pos.y, repeller_strength / 10);

  // draw title
  pen(15, 15, 15, 12); frect(0, 0, 240, 9);
  pen(5, 7, 9); text("SMOKE + REPELLER", 2, 2);

  // draw menu / state
  pen(15, 15, 15);
  cursor(2, 12);
  text(std::string("BLEND: ") + (blender == ALPHA ? "ALPHA": "CUSTOM") + "   \\pen4cffB");
  text(std::string("SPEED: ") + str(speed) + "%   \\pen4cffU/D");
  text(std::string("REPELLER: ") + str(repeller_strength) + "%   \\pen4cffL/R");
  text(std::string("COUNT: ") + str(smoke.max_count) + "   \\pen4cffX");
  text(std::string("RESET   \\pen4cffY"));

  // draw fps
  pen(4, 4, 4, 8); frect(0, 111, 120, 9);
  pen(15, 15, 15); text("FPS: " + str(stats.fps), 2, 113);
}