#include "picosystem.hpp"

using namespace picosystem;

// initialise the world
void init() {
}

// process user input and update the world state
void update(uint32_t tick) {
}

// draw the world
void draw() {
  // clear the screen in noxious 3310 backlight green and draw everything in
  // a faint blended black to get that cheap 90s LCD feel
  pen(10, 12, 0);
  clear();

  pen(0, 0, 0, 4);
  std::string test = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. The Quick Brown Fox Jumped Over The Lazy Dog. 1234567890 ?><,.;'# []{}-=_+ \\|";
  int wr = (fsin(time() / 1000.0f) * 30.0f) + 60;
  uint32_t s = time_us();
  wrap(test, wr);
  text(str(time_us() - s), 50, 50);
  text(test, 4, 20);
  rect(4, 20, wr, 140);
}