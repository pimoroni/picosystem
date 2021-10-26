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
    case 4: {
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

    case 0: {
      pen(0, 0, 0);
      clear();

      title("Effects");

      auto centre_text = [](std::string message, int32_t y, int32_t xo = 0) {
        int32_t w, h;
        measure(message, w, h);
        text(message, 60 - (w / 2) + xo, y);
      };

      std::string m;

      // shadow text
      m = "Shadow text example";
      blend(PEN);
      pen(5, 5, 6);
      centre_text(m, 23);
      blend(MASK);
      pen(13, 13, 15);
      centre_text(m, 20);

      // gradient text
      m = "Gradient text example";
      blend(MASK);
      pen(14, 12, 0);
      clip(0, 40, 120, 2);
      centre_text(m, 40);
      pen(14, 10, 0);
      clip(0, 42, 120, 2);
      centre_text(m, 40);
      pen(12, 8, 0);
      clip(0, 44, 120, 2);
      centre_text(m, 40);
      pen(10, 4, 0);
      clip(0, 46, 120, 2);
      centre_text(m, 40);
      clip();

      // outline text
      m = "Outline text example";
      blend(MASK);
      pen(0, 12, 0);
      centre_text(m, 59, -1);
      centre_text(m, 60, -1);
      centre_text(m, 61, -1);
      centre_text(m, 59, 0);
      centre_text(m, 60, 0);
      centre_text(m, 61, 0);
      centre_text(m, 59, 1);
      centre_text(m, 60, 1);
      centre_text(m, 61, 1);
      pen(0, 0, 0);
      centre_text(m, 60, 0);

      // split text
      m = "Split text example";
      blend(MASK);
      pen(12, 12, 15);
      clip(0, 80, 120, 5);
      centre_text(m, 80);
      pen(15, 12, 15);
      clip(0, 85, 120, 3);
      centre_text(m, 80);
      clip();


      // glow text
      m = "Glow text example";
      blend(ALPHA);
      pen(15, 15, 15, 2);
      centre_text(m, 99, -1);
      centre_text(m, 100, -1);
      centre_text(m, 101, -1);
      centre_text(m, 99, 0);
      centre_text(m, 100, 0);
      centre_text(m, 101, 0);
      centre_text(m, 99, 1);
      centre_text(m, 100, 1);
      centre_text(m, 101, 1);
      blend(MASK);
      pen(0, 0, 0);
      centre_text(m, 100, 0);


/*
      blend(ALPHA);
      sprite(SKULL, 52, 30, 1, 1, 16, 16);

      pen(15, 12, 8);
      shadow_text("Game Over", 50);

      alpha(sin(time() / 200.0f) * 2.0f + 10.0f);
      pen(15, 15, 15);
      shadow_text("Try again: Y/N?", 90);
      alpha();*/

    }break;
  }
}