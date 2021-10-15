

#include "picosystem.hpp"
#include "hardware/pwm.h"
#include "math.h"

using namespace picosystem;

struct dial_t {
  std::string name, unit;
  int32_t min, max, value, step;
};

std::vector<dial_t> dials;
uint32_t active_dial = 0;

dial_t *get_dial(std::string name) {
  for(auto &d :dials) {
    if(d.name == name) return &d;
  }
  return nullptr;
}

void adjust_dial_value(dial_t &d, int32_t delta) {
  d.value += delta;
  d.value = d.value > d.max ? d.max : d.value;
  d.value = d.value < d.min ? d.min : d.value;
}

uint32_t get_dial_value(std::string name) {
  dial_t *d = get_dial(name);
  return d->value;
}

voice_t v = {
  .frequency  =  440,   // frequency in hz
  .bend       =    0,   // amount to increase frequency by every bend ms
  .bend_ms    =   10,   // time step between frequency bends
  .attack     =  500,   // attack time in ms
  .decay      =  250,   // decay time in ms
  .hold       =  800,   // hold time in ms
  .release    =  500,   // release time in ms
  .reverb     =    0,   // reverb timing in ms
  .sustain    =   80,   // sustain volume (0..100)
  .volume     =  100,   // overall volume (0..100)
  .noise      =  100,   // additive noise for each sample (0..100)
  .distort    =    0    // level of bitcrushing to apply (0..100)
};

uint32_t draw_tick = 0;

void set_voice() {
  v.frequency = get_dial_value("frequency");
  v.volume    = get_dial_value(   "volume");
  v.sustain   = get_dial_value(  "sustain");
  v.distort   = get_dial_value(  "distort");
  v.attack    = get_dial_value(   "attack");
  v.decay     = get_dial_value(    "decay");
  v.hold      = get_dial_value(     "hold");
  v.release   = get_dial_value(  "release");
  v.reverb    = get_dial_value(   "reverb");
  v.bend      = get_dial_value(     "bend");
  v.bend_ms   = get_dial_value(  "bend ms");
  v.noise     = get_dial_value(    "noise");
  play(v);
}

// initialise the world
void init() {
  dials.push_back(
    {.name = "frequency", .unit = "hz", .min = 0, .max = 8000, .value = 440, .step = 50});
  dials.push_back(
    {.name =    "volume", .unit =  "%", .min = 0, .max =  100, .value = 100, .step = 10});
  dials.push_back(
    {.name =    "sustain", .unit = "%", .min = 0, .max =  100, .value =  80, .step = 10});
  dials.push_back(
    {.name =    "distort", .unit = "%", .min = 0, .max =  100, .value =   0, .step = 10});
  dials.push_back(
    {.name =    "attack", .unit = "ms", .min = 0, .max = 1000, .value = 100, .step = 50});
  dials.push_back(
    {.name =     "decay", .unit = "ms", .min = 0, .max = 1000, .value =  50, .step = 50});
  dials.push_back(
    {.name =      "hold", .unit = "ms", .min = 0, .max = 1000, .value = 500, .step = 50});
  dials.push_back(
    {.name =   "release", .unit = "ms", .min = 0, .max = 1000, .value = 250, .step = 50});
  dials.push_back(
    {.name =    "reverb", .unit = "ms", .min = 0, .max = 2000, .value =   0, .step = 50});
  dials.push_back(
    {.name =      "bend", .unit = "hz", .min = 0, .max =  100, .value =   0, .step = 10});
  dials.push_back(
    {.name =   "bend ms", .unit = "ms", .min = 0, .max = 1000, .value = 100, .step = 50});
  dials.push_back(
    {.name =     "noise", .unit =  "%", .min = 0, .max =  100, .value =   0, .step = 10});

  set_voice();
}

// process user input and update the world state
void update(uint32_t tick) {

  bool change = false;

  if(tick % 10 == 0) {
    if(button(UP)) {
      adjust_dial_value(dials[active_dial], dials[active_dial].step);
      change = true;
    }

    if(button(DOWN)) {
      adjust_dial_value(dials[active_dial], -dials[active_dial].step);
      change = true;
    }
  }

  if(pressed(LEFT)) {
    active_dial = active_dial == 0 ? dials.size() - 1 : active_dial - 1;
  }

  if(pressed(RIGHT)) {
    active_dial = active_dial < dials.size() - 1 ? active_dial + 1 : 0;
  }

  uint32_t duration = v.attack + v.decay + v.hold + v.release + v.reverb;
  if(change || audio_position() > duration) {
    set_voice();
  }
}


void draw_dial(std::string name, int32_t x, int32_t y) {
  dial_t *d = get_dial(name);

  pen(0, 2, 3, 2);
  frect(x, y, 60, 60);
  rect(x, y, 60, 60);

  pen(15, 15, 15, 8);
  fcircle(x + 4, y + 4, 1);
  fcircle(x + 60 - 4, y + 4, 1);
  fcircle(x + 4, y + 60 - 4, 1);
  fcircle(x + 60 - 4, y + 60 - 4, 1);

  pen(0, 2, 3, 8);
  fcircle(x + 3, y + 3, 1);
  fcircle(x + 60 - 3, y + 3, 1);
  fcircle(x + 3, y + 60 - 3, 1);
  fcircle(x + 60 - 3, y + 60 - 3, 1);

  pen(0, 2, 3, 2);
  fcircle(x + 33, y + 23, 18);

  pen(0, 2, 3);
  fcircle(x + 30, y + 20, 18);

  pen(15, 15, 12);
  fcircle(x + 30, y + 20, 14);

  // draw "dot" of the dial
  float fv = float(d->value) / (d->max - d->min);
  int32_t dotx = -sin(fv * 3.1415927 * 2.0f) * 10;
  int32_t doty = cos(fv * 3.1415927 * 2.0f) * 10;

  pen(0, 2, 3);
  fcircle(dotx + x + 30, doty + y + 20, 3);

  uint32_t lw = text_width(d->name);
  text(d->name, x + 30 - (lw / 2), y + 60 - 20);
  std::string unit_label = str(d->value) + d->unit;
  lw = text_width(unit_label);
  text(unit_label, x + 30 - (lw / 2), y + 60 - 10);

  if(d == &dials[active_dial]) {
    pen(0, 15, 0);
    rect(x, y, 60, 60);
    rect(x + 1, y + 1, 58, 58);
  }
}

// draw the world
void draw() {
  pen(12, 12, 2);
  frect(0, 60, 240, 60);
  draw_dial("frequency", 0, 60);
  draw_dial("volume", 60, 60);
  draw_dial("sustain", 120, 60);
  draw_dial("distort", 180, 60);

  pen(2, 12, 12);
  frect(0, 120, 240, 60);
  draw_dial("attack", 0, 120);
  draw_dial("decay", 60, 120);
  draw_dial("hold", 120, 120);
  draw_dial("release", 180, 120);

  pen(12, 2, 12);
  frect(0, 180, 240, 60);
  draw_dial("reverb", 0, 180);
  draw_dial("bend", 60, 180);
  draw_dial("bend ms", 120, 180);
  draw_dial("noise", 180, 180);

  // draw waveform graph
  pen(0, 0, 0);
  frect(0, 0, 240, 60);

  uint32_t duration = v.attack + v.decay + v.hold + v.release + v.reverb;
  pen(15, 15, 15);
  for(int i = 0; i < 230; i+=4) {
    uint8_t s = audio_sample((i * duration) / 230);
    //pixel(i + 5, 55 - s / 2);
    frect(i + 5, 55 - s / 2, 3, 3);
  }

  // draw current playback marker
  pen(0, 15, 15);
  uint32_t pos = (audio_position() * 230) / duration;
  frect(pos + 5, 5, 2, 50);
}


