

#include "picosystem.hpp"
#include "hardware/pwm.h"
#include "math.h"

using namespace picosystem;

// initialise the world
void init() {
}

voice_t v = {
  .frequency  =  440,   // frequency in hz
  .bend       =    1,   // amount to increase frequency by every 10 ms
  .attack     =  500,   // attack time in ms
  .decay      =  250,   // decay time in ms
  .hold       =  800,   // hold time in ms
  .release    =  500,   // release time in ms
  .reverb     = 2000,   // reverb timing in ms
  .sustain    =   80,   // sustain volume (0..100)
  .volume     =  100,   // overall volume (0..100)
  /* .noise = 100,*/    // additive noise for each sample (0..100)
  .distort    =    0    // level of bitcrushing to apply (0..100)
};

uint32_t draw_tick = 0;

// process user input and update the world state
void update(uint32_t tick) {

  if(pressed(UP)) {
    v.frequency += 10;
  }

  if(pressed(DOWN)) {
    v.frequency -= 10;
  }

  if(pressed(RIGHT)) {
    v.volume = v.volume < 100 ? v.volume + 10 : 100;
  }

  if(pressed(LEFT)) {
    v.volume = v.volume > 0 ? v.volume - 10 : 0;
  }

  if(tick % 300 == 0) {
    draw_tick = 0;
    play(v);
  }
}

// draw the world
void draw() {
  // clear the screen in noxious 3310 backlight green and draw everything in
  // a faint blended black to get that cheap 90s LCD feel
  pen(0, 4, 0);
  clear();

  pen(15, 15, 15);

  text("frequency:  " + str(v.frequency), 10, 20);
  text("volume:  " + str(v.volume), 10, 30);
  text(str(_debug), 10, 40);

  pixel(draw_tick * 2, 180 - (last_audio_sample() / 2));
  draw_tick++;
}


