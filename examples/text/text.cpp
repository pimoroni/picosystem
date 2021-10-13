#include "picosystem.hpp"

using namespace picosystem;

uint32_t wrap_width;

void init() {
}

void update(uint32_t tick) {
  wrap_width = (fsin(time() / 1000.0f) * 40.0f) + 76;
}

void draw() {
  pen(0, 0, 0);
  clear();

  // draw title
  pen(15, 15, 15);
  frect(0, 0, 120, 11);
  pen(0, 0, 0);
  text("Word Wrap Test", 2, 2);

  pen(15, 15, 15);
  text("Wrap at " + str(wrap_width) + " pixels:", 2, 15);

  // define a long message to word wrap
  std::string message = "\
THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. \
The Quick Brown Fox Jumped Over The Lazy Dog. \
1234567890 ?><,.;'# []{}-=_+ \\|";

  // given the message, and a width in pixels, the wrap() method will insert
  // newline characters where needed (modifying the original string)
  wrap(message, wrap_width);

  // draw wrapped text
  pen(8, 8, 8);
  text(message, 2, 28);

  // draw green box to show wrap width
  pen(0, 12, 0);
  rect(1, 27, wrap_width + 2, 92);
}