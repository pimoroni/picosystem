#include "picosystem.hpp"

using namespace picosystem;

uint32_t wrap_width;

void init() {
  //int32_t w = -1, int32_t h = 8, int32_t s = 1,
  //              uint8_t *data = nullptr
 // font(4, 6, 1);
}

void update(uint32_t tick) {
  wrap_width = (fsin(time() / 1000.0f) * 20.0f) + 96;
}

void draw(uint32_t tick) {
  pen(0, 0, 0, 2);
  clear();

  // draw title
  pen(15, 15, 15);
  frect(0, 0, 120, 11);
  pen(0, 0, 0);
  text("Word wrap test", 2, 2);

  pen(15, 15, 15);
  text("Wrap at " + str(wrap_width) + " pixels:", 2, 15);

  // define a long message to word wrap
  std::string message = "\\rgbcccWe are just an advanced breed of \\rgbc0cmonkeys\\rgbccc on a minor planet of a \\rgbcc0very average star\\rgbccc. But we can \\rgb0ccunderstand\\rgbccc the Universe. That makes us something very special.\
  \n\
  \n\t\\rgbfff- Stephen Hawking";

  int32_t w, h;

  uint32_t s = time_us();
  measure(message, w, h, wrap_width);
  pen(0, 0, 0);
  text(str(time_us() - s), 100, 2);

  // draw measured rectangle for text
  pen(4, 4, 4, 4);
  frect(2, 28, w, h);

  // draw wrapped text
  pen(8, 8, 8);
  text(message, 2, 28, wrap_width);

  // draw green line to show wrap width
  pen(0, 12, 0);
  vline(wrap_width + 2, 27, 92);
}