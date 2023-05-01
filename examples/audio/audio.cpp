#include <math.h>
#include <array>

#include "picosystem.hpp"


using namespace picosystem;

// a place to store the current state of the dials
struct dial_t {
  std::string name, unit;
  int32_t min, max, value, step;
};

std::array<dial_t, 12> dials = {{
  {"frequency", "hz",    0, 8000, 440, 50},
  {   "volume",  "%",    0,  100, 100, 10},
  {  "sustain",  "%",    0,  100,  80, 10},
  {  "distort",  "%",    0,  100,   0, 10},
  {   "attack", "ms",    0, 1000, 100, 50},
  {    "decay", "ms",    0, 1000,  50, 50},
  { "duration", "ms",    0, 1000, 500, 50},
  {  "release", "ms",    0, 1000, 250, 50},
  {   "reverb", "ms",    0, 2000,   0, 50},
  {     "bend", "hz", -100,  100,   0, 10},
  {  "bend ms", "ms",    0, 1000, 100, 50},
  {    "noise",  "%",    0,  100,   0, 10}
}};

// index of the selected diable
uint32_t selected = 0;

// return a dial by name
dial_t *get_dial(std::string name) {
  for(auto &d :dials) {
    if(d.name == name) return &d;
  }
  return nullptr;
}

// change a dial value up or down and clamp to limits
void adjust_dial_value(dial_t &d, int32_t delta) {
  d.value += delta;
  d.value = d.value > d.max ? d.max : d.value;
  d.value = d.value < d.min ? d.min : d.value;
}

// return the current value of the named dial
uint32_t get_dial_value(std::string name) {
  dial_t *d = get_dial(name);
  return d->value;
}

// voice for the current dial settings
voice_t v;

// update and play the voice
void set_voice() {
  v = voice(
    get_dial_value(   "attack"),
    get_dial_value(    "decay"),
    get_dial_value(  "sustain"),
    get_dial_value(  "release"),
    get_dial_value(     "bend"),
    get_dial_value(  "bend ms"),
    get_dial_value(   "reverb"),
    get_dial_value(    "noise"),
    get_dial_value(  "distort"));

  play(v, get_dial_value("frequency"), get_dial_value("duration"), get_dial_value("volume"));
}

// initialise the world
void init() {
  set_voice();
}

// process user input and update the world state
void update(uint32_t tick) {
  bool change = false;

  // only check for UP/DOWN state every 10 ticks (100ms) to ensure that dial
  // values change at a sensible rate
  if(tick % 10 == 0) {
    if(button(UP)) {
      adjust_dial_value(dials[selected], dials[selected].step);
      change = true;
    }
    if(button(DOWN)) {
      adjust_dial_value(dials[selected], -dials[selected].step);
      change = true;
    }
  }

  // if LEFT or RIGHT has been pressed then move selection to the next or
  // previous dial
  if(pressed(LEFT)) {
    selected = selected == 0 ? dials.size() - 1 : selected - 1;
  }
  if(pressed(RIGHT)) {
    selected = selected < dials.size() - 1 ? selected + 1 : 0;
  }

  // if the current sound has finished playing, or we changed a dial value
  // then restart the sound
  uint32_t full_duration = get_dial_value("duration") + get_dial_value("release") + get_dial_value("reverb");
  if(change || position() > full_duration) {
    set_voice();
  }
}

void draw_dial(std::string name, int32_t x, int32_t y) {
  dial_t *d = get_dial(name);

  // draw outline
  pen(0, 0, 0, 2);
  rect(x, y, 60, 60);

  // draw rivets
  pen(0, 0, 0, 2);
  fcircle(x + 3, y + 3, 1);
  fcircle(x + 60 - 4, y + 3, 1);
  fcircle(x + 3, y + 60 - 4, 1);
  fcircle(x + 60 - 4, y + 60 - 3, 1);

  // draw dial face
  pen(0, 0, 0, 4);
  fcircle(x + 30, y + 20, 16);
  pen(15, 15, 15, 12);
  fcircle(x + 30, y + 20, 13);

  // draw dot of the dial
  float fv = float(d->value + abs(d->min)) / (d->max - d->min);
  int32_t dotx = -sin(3.1415927 * 0.2f + fv * 3.1415927 * 1.6f) * 9;
  int32_t doty = cos(3.1415927 * 0.2f + fv * 3.1415927 * 1.6f) * 9;
  pen(0, 0, 0, 4);
  fcircle(dotx + x + 30, doty + y + 20, 3);

  // centre dial label and value
  uint32_t lw = 60; //text_width(d->name);
  text(d->name, x + 30 - (lw / 2), y + 60 - 22);
  std::string unit_label = str(d->value) + d->unit;
  lw = 20;//text_width(unit_label);
  text(unit_label, x + 30 - (lw / 2), y + 60 - 12);

  // if this is the active dial then highlight it
  if(d == &dials[selected]) {
    pen(0, 15, 0);
    rect(x, y, 60, 60);
    rect(x + 1, y + 1, 58, 58);
  }
}

// draw the world
void draw(uint32_t tick) {
  // draw top row of dials
  pen(10, 12, 4);
  frect(0, 60, 240, 60);
  draw_dial("frequency", 0, 60);
  draw_dial("volume", 60, 60);
  draw_dial("sustain", 120, 60);
  draw_dial("distort", 180, 60);

  // draw middle row of dials
  pen(4, 10, 12);
  frect(0, 120, 240, 60);
  draw_dial("attack", 0, 120);
  draw_dial("decay", 60, 120);
  draw_dial("duration", 120, 120);
  draw_dial("release", 180, 120);

  // draw bottom row of dials
  pen(12, 4, 10);
  frect(0, 180, 240, 60);
  draw_dial("reverb", 0, 180);
  draw_dial("bend", 60, 180);
  draw_dial("bend ms", 120, 180);
  draw_dial("noise", 180, 180);

  // draw waveform graph
  pen(0, 0, 0, 2);
  frect(0, 0, 240, 60);
  uint32_t full_duration = get_dial_value("duration") + get_dial_value("release") + get_dial_value("reverb");
  pen(15, 15, 15);
  for(int i = 0; i < 230; i+=6) {
    uint8_t s = audio_sample((i * full_duration) / 230);
    frect(i + 2, 52 - s / 2, 5, 5);
  }

  // draw current playback marker
  pen(0, 15, 15);
  uint32_t pos = (position() * 230) / full_duration;
  frect(pos + 5, 5, 2, 50);
}