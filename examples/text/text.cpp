#include <math.h>
#include "picosystem.hpp"

using namespace picosystem;

uint32_t view = 0;
uint32_t view_count = 5;

void init() {
}

void update(uint32_t tick) {
  if(pressed(RIGHT)) {view = view == (view_count - 1) ? 0 : view + 1;}
  if(pressed( LEFT)) {view = view == 0 ? (view_count - 1) : view - 1;}
}

void title(std::string t) {
  pen(15, 15, 15);
  frect(0, 0, 120, 11);
  pen(0, 0, 0);
  text(t + " (" + str(view + 1) + "/" + str(view_count) + ")", 2, 2);
}

extern color_t image_data[1440];
buffer_t douglas = {.w = 32, .h = 45, .data = image_data};

void draw(uint32_t tick) {
  pen(0, 0, 0);
  clear();

  switch(view) {
    case 0: {
      title("Word wrap");

      uint32_t wrap = (sin(time() / 1000.0f) * 36.0f) + 80;

      font();

      // define a long message to word wrap
      std::string message = "\
We are just an advanced breed of monkeys on a minor \
planet of a very average star. But we can understand the \
Universe. That makes us something very special.\
\n\
\n\t- Stephen Hawking";

      // measure how large the text will be and draw boundary on screen
      int32_t w, h;
      measure(message, w, h, wrap);
      pen(0, 1, 0);
      frect(0, 28, w + 4, h + 4);

      // draw wrapped text
      pen(12, 12, 12);
      text(message, 2, 30, wrap);

      // draw wrap width marker
      text("wrap", wrap - 24, 15);
      vline(wrap + 2, 15, 8);
      line(wrap + 2, 23, wrap + 2 - 2, 21);
      line(wrap + 2, 23, wrap + 2 + 2, 21);
      pen(0, 15, 0);
      vline(wrap + 2, 27, 92);
    } break;

    case 1: {
      title("Colour codes");

      font(-1, 12);

      pen(15, 15, 15);
      std::string message = "\
By using special codes you \
can change to any colour in the \
\\penf00fR\\penf80fA\\penff0fI\\pen0f0fN\\pen00ffB\\pen80ffO\\penf0ffW";

      pen(12, 12, 12);
      text(message, 2, 32, 100);
    }break;

    case 2: {
      font(-1, 12);

      std::string message = "\
\"The fact that we live at the bottom of a deep gravity well, on the surface of \
a gas covered planet going around a nuclear fireball 90 million miles away and \
think this to be normal is obviously some indication of how skewed our \
perspective tends to be.\"";

      // box width and height
      uint32_t bx = 10, by = 20;
      uint32_t bw = 100, bh = 90;

      int32_t w, h, p = 20;
      measure(message, w, h, bw - 10);
      h += p * 2;
      int32_t overflow = h - bh;
      int32_t scroll = (sin(time() / 2000.0f) * overflow / 2.0f) + (overflow / 2.0f);
      scroll -= p;

      pen(15, 0, 0);
      rect(bx - 2, by - 2, bw + 4, bh + 4);
      clip(bx, by, bw, bh);
      pen(12, 12, 12);
      text(message, bx, by - scroll, bw);
      clip();

      title("Scroll and clip");
    }break;

    case 3: {
      font(-1, 12);

      std::string message = "\
Lo, brave adventurer! Choose a tasty treat:\
\n\
\n   A) Ripe and tasty \\spr001\
\n   B) Delicious \\spr000\
\n   C) This firm \\spr004\
\n   D) Very spooky \\spr013\
\n\
\nMake your choice...";

      pen(12, 12, 12);
      text(message, 2, 16, 116);

      title("Inline icons");
    }break;

    case 4: {
      pen(0, 0, 0);
      clear();

      // little helper function that centres and shadows text at the supplied
      // y coordinate
      auto shadow_text = [](std::string message, int32_t y) {
        int32_t tx, ty;
        measure(message, tx, ty);

        color_t old_pen = _pen;
        pen(3, 3, 3);
        text(message, 60 - (tx / 2), y + 1);
        text(message, 60 - (tx / 2), y + 2);
        pen(old_pen);
        text(message, 60 - (tx / 2), y);
      };

      title("Shadow");

      blend(PEN);
      pen(3, 3, 3);
      sprite(SKULL, 52, 32, 1, 1, 16, 16);

      blend(ALPHA);
      sprite(SKULL, 52, 30, 1, 1, 16, 16);

      pen(15, 12, 8);
      shadow_text("Game Over", 50);

      alpha(sin(time() / 200.0f) * 2.0f + 10.0f);
      pen(15, 15, 15);
      shadow_text("Try again: Y/N?", 90);
      alpha();

    }break;
  }
}