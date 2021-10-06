#include <string.h>
#include <math.h>
#include <vector>

#include "picosystem.hpp"

using namespace picosystem;

struct vec_t {
  int32_t x, y;
};

// each square of the map is 4x4 pixels which on a 120x120 display
// means the map is 60x60 squares large
constexpr uint32_t scale = 8;
constexpr vec_t map_size{.x = 120 / scale, .y = 120 / scale};

enum state_t {PLAYING, PAUSED, GAME_OVER};
state_t state = PLAYING;

// current position and direction
struct {
  vec_t dir;
  uint32_t length;
  std::vector<vec_t> body;

  void reset() {
    // place the player back in the centre of the map and remove it's tail
    vec_t start = {.x = map_size.x / 2, .y = map_size.y / 2};
    body.clear();
    body.insert(body.begin(), start);
    dir.x = 1; dir.y = 0;
    length = 1;
  }

  void move() {
    // add new location to front of body
    vec_t pos = body[0];
    pos.x += dir.x; pos.y += dir.y;
    body.insert(body.begin(), pos);

    // if snake tail is too long now then remove last element
    if(body.size() > length) { body.pop_back(); }
  }

  vec_t head() {
    return body[0];
  }
} snake;

vec_t apple;

// place the apple in a new random location which is not the snake location
// or part of the snakes tail
void place_apple() {
  bool hit = false;
  do {
    // generate a new location for the apple
    apple.x = (std::rand() % (map_size.x - 2)) + 1;
    apple.y = (std::rand() % (map_size.y - 2)) + 1;

    // check new apple location not in snake position or snakes tail
    for(auto v : snake.body) {
      hit |= v.x == apple.x && v.y == apple.y;
    }
  } while(hit);
}

// initialise the world
void init() {
  state = PLAYING;
  snake.reset();
  place_apple();
}

// each tick is 10ms
void update(uint32_t tick) {
  if(state == PLAYING) {
    // player input, you can't reverse direction - for example changing
    // direction to "up" cannot occur if you're currently pointing "down"
    if(pressed(UP)    && !snake.dir.y) {snake.dir.x =  0; snake.dir.y = -1;}
    if(pressed(DOWN)  && !snake.dir.y) {snake.dir.x =  0; snake.dir.y =  1;}
    if(pressed(LEFT)  && !snake.dir.x) {snake.dir.x = -1; snake.dir.y =  0;}
    if(pressed(RIGHT) && !snake.dir.x) {snake.dir.x =  1; snake.dir.y =  0;}

    // every 10 (100ms) ticks we'll update position
    if(tick % 10 == 0) {
      snake.move();

      vec_t head = snake.head();

      // check if the snakes head is on the apple, if so make longer and
      // move the apple
      if(head.x == apple.x && head.y == apple.y) {
        snake.length++;
        place_apple();
      }

      // check if the snakes head has collided with the walls
      if(head.x <= 0 || head.x >= map_size.x - 1 ||
         head.y <= 0 || head.y >= map_size.y - 1) {
        state = GAME_OVER;
      }

      // check if the snakes head has collided with it's tail
      for(uint32_t i = 1; i < snake.body.size() - 1; i++) {
        vec_t b = snake.body[i];
        if(b.x == head.x && b.y == head.y) {
          state = GAME_OVER;
        }
      }
    }

    if(pressed(X)) {state = PAUSED;}
  } else if(state == PAUSED) {
    if(pressed(X)) {state = PLAYING;}
  } else if(state == GAME_OVER) {
    if(pressed(A)) {
      init();
    }
  }
}

// fills a single square on the map in the provided colour
void tile(color_t col, int32_t x, int32_t y) {
  pen(col);
  rect(x * scale, y * scale, scale, scale);
}

void draw() {
  // clear the screen
  pen(0, 0, 0);
  clear();

  // draw the walls
  uint32_t size = map_size.x;
  for(int i = 0; i < size; i++) {
    tile(color(5, 6, 7), i, 0);
    tile(color(5, 6, 7), i, map_size.y - 1);
    tile(color(5, 6, 7), 0, i);
    tile(color(5, 6, 7), map_size.x - 1, i);
  }

  // draw the apple
  tile(color(15, 0, 0), apple.x, apple.y);

  // draw the snake
  for(uint32_t i = 0; i < snake.body.size(); i++) {
    vec_t p = snake.body[i];
    tile(color(0, i == 0 ? 15 : 7, 0), p.x, p.y);
  }

  pen(15, 15, 15);
  text(str(snake.length), 0, 0);

  // if pause then overlay a message
  if(state == PAUSED) {
    pen(4, 4, 4, 8);
    rect(0, 0, 120, 120);

    pen(15, 15, 15);
    text("PAUSED", 10, 10);
  }

  if(state == GAME_OVER) {
    pen(15, 0, 0, 8);
    rect(0, 0, 120, 120);

    pen(15, 15, 15);
    text("GAME OVER", 10, 10);
    text("YOU SCORED", 10, 20);
    text(str(snake.length), 10, 30);

    text("PRESS A TO", 10, 50);
    text(" RESTART  ", 10, 60);
  }
}