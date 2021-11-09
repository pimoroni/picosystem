#include <array>
#include <math.h>

#include "picosystem.hpp"

using namespace picosystem;

struct shape_t {
  int32_t x, y, w, h, r;
  color_t p;
};

std::array<shape_t, 250> shapes;

void reset() {
  for(auto &s : shapes) {
    s.x = std::rand() % SCREEN->w;
    s.y = std::rand() % SCREEN->h;
    s.w = std::rand() % 40;
    s.h = std::rand() % 40;
    s.r = std::rand() % 360;
    s.p = hsv((std::rand() % 100) / 100.0f, 1.0f, 1.0f);
  }
}

void move() {
  for(auto &s : shapes) {
    s.x += std::rand() % 3 - 1;
    s.y += std::rand() % 3 - 1;
    s.w += std::rand() % 3 - 1;
    s.h += std::rand() % 3 - 1;
    s.r += 3;
  }
}

void init() {
  reset();
}

int32_t view = -1;


void update(uint32_t tick) {
  move();

  if(!(tick % 250)) {
    reset();
    view++;

    if(view > 9) {
      view = 0;
    }
  }
}

void label(std::string s) {
  blend(ALPHA);
  pen(1, 2, 3, 8);
  frect(0, 11, SCREEN->w, 15);
  pen(0, 0, 0);
  text(s + ":", 3, 16);
  pen(15, 15, 15);
  text(s + ":", 2, 15);
}

void draw(uint32_t tick) {
  uint32_t start = time_us();
  pen(0, 0, 0);
  clear();

  switch(view) {
    case 0: {
      alpha(8);
      for(auto &s : shapes) {pen(s.p); frect(s.x, s.y, s.w, s.h);}
      alpha();
      label("Filled rectangles");
    } break;
    case 1: {
      for(auto &s : shapes) {pen(s.p); line(s.x, s.y, s.x + s.w, s.y + s.h);}
      label("Lines");
    } break;
    case 2: {
      uint32_t i = 0;
      for(auto &s : shapes) {
        pen(s.p);
        text(std::string(1, char((i++ % 96) + 32)), s.x, s.y);
      }
      label("Text");
    } break;
    case 3: {
      uint32_t i = 0;
      for(auto &s : shapes) {pen(s.p); ellipse(s.x, s.y, s.w / 2, s.h / 2);}
      label("Ellipses");
    } break;
    case 4: {
      blend(MASK);
      uint32_t i = 0;
      for(auto &s : shapes) {sprite(i++, s.x, s.y);}
      label("Sprites");
    } break;
    case 5: {
      for(auto &s : shapes) {
        pen(s.p);
        int32_t rx = sin(s.r * (3.1415927f / 180.0f)) * (s.w / 2);
        int32_t ry = cos(s.r * (3.1415927f / 180.0f)) * (s.h / 2);
        fpoly({
          s.x - rx, s.y - ry, s.x + ry, s.y - rx,
          s.x + rx, s.y + ry, s.x - ry, s.y + rx
        });
      }
      label("Filled polygons");
    } break;
    case 6: {
      for(auto &s : shapes) {pen(s.p); circle(s.x, s.y, s.w / 2);}
      label("Circles");
    } break;
    case 7: {
      uint32_t i = 0;
      for(auto &s : shapes) {pen(s.p); fellipse(s.x, s.y, s.w / 2, s.h / 2);}
      label("Filled Ellipses");
    } break;
    case 8: {
      for(auto &s : shapes) {pen(s.p); rect(s.x, s.y, s.w, s.h);}
      label("Rectangles");
    } break;
    case 9: {
      for(auto &s : shapes) {pen(s.p); fcircle(s.x, s.y, s.w / 2);}
      label("Filled circles");
    } break;
    case 10: {
      for(auto &s : shapes) {
        pen(s.p);
        int32_t rx = sin(s.r * (3.1415927f / 180.0f)) * (s.w / 2);
        int32_t ry = cos(s.r * (3.1415927f / 180.0f)) * (s.h / 2);
        poly({
          s.x - rx, s.y - ry, s.x + ry, s.y - rx,
          s.x + rx, s.y + ry, s.x - ry, s.y + rx
        });
      }
      label("Polygons");
    } break;
    case 11: {
      for(auto &s : shapes) {
        pen(s.p);
        pixel(s.x, s.y);
      }
      label("Pixels");
    } break;
  }

  blend(ALPHA);

  // draw title
  pen(15, 15, 15);
  frect(0, 0, SCREEN->w, 11);
  pen(0, 0, 0);
  text("Shapes Test", 2, 2);

  // draw stats
  pen(1, 2, 3, 8);
  frect(0, SCREEN->h - 74, 80, 74);

  pen(15, 15, 15);
  cursor(2, SCREEN->h - 70);
  text("shapes: " + str(shapes.size()));
  text("fps: " + str(stats.fps));
  text("draw us: " + str(stats.draw_us));
  text("update us: " + str(stats.update_us));
  text("tick us: " + str(stats.tick_us));
  text("idle: " + str(stats.idle) + "%");
  text("flip us: " + str(stats.flip_us));
}