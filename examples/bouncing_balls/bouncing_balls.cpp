#include <string.h>
#include <math.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include "picosystem.hpp"

using namespace picosystem;
/*
struct Ball {
  Vec2 position;
  Vec2 direction;
  int radius;
  Pen pen;
};*/

int main() {
  init_picosystem();
/*
  std::vector<Ball> balls;
  for(int i = 0; i < 1000; i++) {
    Ball ball = {
      .position = Vec2(rand() % 120, rand() % 120),
      .direction = Vec2((float(rand() % 255) / 128.0f) - 1.0f, (float(rand() % 255) / 128.0f) - 1.0f),
      .radius = (rand() % 5) + 2,
      .pen = Pen(rand() % 15, rand() % 15, rand() % 15)
    };
    balls.push_back(ball);
  }

  while(true) {
    screen.pen = Pen(1, 2, 3);
    screen.clear();

    for(auto &ball : balls) {
      ball.position += ball.direction;

      if(ball.position.x < 0)         ball.direction.x *= -1;
      if(ball.position.x >= screen.w) ball.direction.x *= -1;
      if(ball.position.y < 0)         ball.direction.y *= -1;
      if(ball.position.y >= screen.h) ball.direction.y *= -1;

      screen.pen = ball.pen;
      screen.circle(Point(ball.position), ball.radius);
    }

    float battery = battery_voltage();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << battery;
    std::string voltage_string = ss.str();

    screen.pen = Pen(0, 0, 0, 8);
    screen.rectangle(5, 5, 34, 15);

    screen.pen = Pen(15, 15, 15);
    screen.text(voltage_string, 10, 10);

    flip();
  }*/
}
