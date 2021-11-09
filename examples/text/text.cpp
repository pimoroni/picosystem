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

      static float hue = 0.0f, hue_step = 1.0f / 7.0f;
      color_t c1, c2;
      hue += 0.01f;



      // shadow text
      auto shadow_text = [](std::string m, color_t c, color_t s, int32_t x, int32_t y, int32_t sx, int32_t sy) {
        blend(PEN);
        pen(s);
        text(m, x + sx, y + sy);
        text(m, x + sx - 1, y + sy);
        text(m, x + sx + 1, y + sy);
        text(m, x + sx, y + sy + 1);
        blend(ALPHA);
        pen(c);
        text(m, x, y);
      };
      int32_t sx = sin(time() / 250.0f) * 3.0f;
      c1 = hsv(hue + (hue_step * 0), 0.8f, 1.0f);
      shadow_text("Shadow text \\spr005", c1, rgb(7, 7, 9, 4), 15, 20, sx, 2);
      pen(c1); frect(0, 20, 5, 7);

      // gradient text
      auto gradient_text = [](std::string m, color_t c1, color_t c2, int32_t x, int32_t y) {
        blend(MASK);
        for(int i = 0; i < 8; i++) {
          pen(mix(c2, c1, i * 1.875f)); clip(0, y + i, 120, 1); text(m, x, y);
        }
        clip();
      };
      c1 = hsv(hue + (hue_step * 1), 1.0f, 1.0f); c2 = hsv(hue + (hue_step * 1), 0.5f, 0.5f);
      gradient_text("Gradient text \\spr004", c1, c2, 15, 35);
      pen(c1); frect(0, 35, 5, 7);

      // outline text
      auto outline_text = [](std::string m, color_t c, int32_t x, int32_t y, int32_t o) {
        blend(PEN);
        pen(c);

        for(int i = -o; i <= o; i++) {
          for(int j = -o; j <= o; j++) {
            text(m, x + i, y + j);
          }
        }

        blend(MASK);
        pen(0, 0, 0);
        text(m, x, y);
      };
      int32_t o = 1 + ((sin(time() / 250.0f) + 1.0f) * 1.5f);
      c1 = hsv(hue + (hue_step * 2), 0.8f, 1.0f);
      outline_text("Outline text \\spr003", c1, 15, 50, o);
      pen(c1); frect(0, 50, 5, 7);

      // extrude text
      auto extrude_text = [](std::string m, color_t c, color_t s, int32_t x, int32_t y, int32_t l) {
        blend(PEN);
        pen(s);
        for(int i = 1; i <= l; i++) {
          text(m, x + i, y + i);
        }
        blend(ALPHA);
        pen(c);
        text(m, x, y);
      };
      int32_t l = 1 + ((sin(time() / 250.0f) + 1.0f) * 2.0f);
      c1 = hsv(hue + (hue_step * 3), 0.8f, 1.0f); c2 = hsv(hue + (hue_step * 3) + 0.1f, 0.6f, 0.8f);
      extrude_text("Extrude text \\spr007", c1, c2, 15, 65, l);
      pen(c1); frect(0, 65, 5, 7);


      auto split_text = [](std::string m, color_t c1, color_t c2, int32_t x, int32_t y) {
        blend(MASK);
        pen(c1);
        clip(0, y, 120, 4);
        text(m, x, y);
        pen(c2);
        clip(0, y + 4, 120, 4);
        text(m, x, y);
        clip();
      };

      // split text
      float hue2 = hue + 0.5;
      if(hue2 > 1.0f) hue2 -= 1.0f;
      c1 = hsv(hue + (hue_step * 4), 0.8f, 1.0f); c2 = hsv(hue + (hue_step * 4) + 0.1f, 0.4f, 0.6f);
      split_text("Split text \\spr002", c1, c2, 15, 80);
      pen(c1); frect(0, 80, 5, 7);

      // glow text
      auto glow_text = [](std::string m, color_t c, int32_t x, int32_t y) {
        blend(PEN);
        pen(c);
        text(m, x - 1, y - 1); text(m, x    , y - 1); text(m, x + 1, y - 1);
        text(m, x - 1, y    ); text(m, x    , y    ); text(m, x + 1, y    );
        text(m, x - 1, y + 1); text(m, x    , y + 1); text(m, x + 1, y + 1);
        blend(PEN);
        pen(0, 0, 0);
        text(m, x, y);
      };
      int32_t g = 1 + ((sin(time() / 250.0f) + 1.0f) * 2.0f);
      c1 = hsv(hue + (hue_step * 5), 0.8f, 1.0f, 0.2f);
      glow_text("Glow text \\spr001", c1, 15, 95);
      pen(c1); frect(0, 95, 5, 7);

      // glitch text
      auto glitch_text = [](std::string m, color_t c, int32_t x, int32_t y) {
        blend(MASK);
        pen(c);
        for(uint8_t i = 0; i < 8; i++) {
          clip(0, y + i, 120, 1);
          int8_t o = 0;
          if(std::rand() % 8 == 0) {
            o = (std::rand() % 9) - 4;
          }
          text(m, x + o, y);
        }
        clip();
      };

      // split text
      c1 = hsv(hue + (hue_step * 6), 0.8f, 1.0f);
      glitch_text("Glitch text \\spr000", c1, 15, 110);
      pen(c1); frect(0, 110, 5, 7);

    }break;
  }
}