#include <string.h>
#include <math.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include "picosystem.hpp"

using namespace picosystem;

struct ball_t {
  float x, y, dx, dy;
  int r;
  color_t col;
};

std::vector<ball_t> balls;

void init() {
  for(int i = 0; i < 1000; i++) {
    balls.push_back({
      .x = float(rand() % 120),
      .y = float(rand() % 120),
      .dx = (float(rand() % 255) / 128.0f) - 1.0f,
      .dy = (float(rand() % 255) / 128.0f) - 1.0f,
      .r = (rand() % 5) + 2,
      .col = rgb(rand() % 15, rand() % 15, rand() % 15)
    });
  }
}

void update(uint32_t tick) {
  for(auto &ball : balls) {
    if(!button(A)) {
      ball.x += ball.dx;
      ball.y += ball.dy;
    }

    if(ball.x < 0)          ball.dx *= -1;
    if(ball.x >= SCREEN->w) ball.dx *= -1;
    if(ball.y < 0)          ball.dy *= -1;
    if(ball.y >= SCREEN->h) ball.dy *= -1;
  }
}

void draw(uint32_t tick) {
  blend(COPY);
  pen(1, 2, 3);
  clear();

  pen(15, 15, 15);
  for(int i = 1; i < 10; i++) {
    fcircle(i * 12, i * 12, i);
  }


  return;

  for(auto &ball : balls) {
    pen(ball.col);
    fcircle(ball.x, ball.y, ball.r);
  }

  // draw stats
  pen(1, 2, 3, 8);
  frect(0, SCREEN->h - 74, 80, 74);

  pen(15, 15, 15);
  cursor(2, SCREEN->h - 70);
  text("balls: " + str(balls.size()));
  text("fps: " + str(stats.fps));
  text("draw us: " + str(stats.draw_us));
  text("update us: " + str(stats.update_us));
  text("tick us: " + str(stats.tick_us));
  text("idle: " + str(stats.idle) + "%");
  text("flip us: " + str(stats.flip_us));

}
