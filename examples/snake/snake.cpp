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
constexpr uint32_t scale = 6;
constexpr vec_t bounds{.x = 18, .y = 16};

enum state_t {PLAYING, PAUSED, GAME_OVER};
state_t state = PLAYING;

// current position and direction
struct {
  vec_t dir;
  uint32_t length;
  std::vector<vec_t> body;

  void reset() {
    // place the player back in the centre of the map and remove it's tail
    vec_t start = {.x = bounds.x / 2, .y = bounds.y / 2};
    body.clear();
    body.insert(body.begin(), start);
    dir.x = 1; dir.y = 0;
    length = 1;
  }

  vec_t next_pos() {
    vec_t p = body[0];
    p.x += dir.x; p.y += dir.y;
    return p;
  }

  void move() {
    // add new location to front of body
    body.insert(body.begin(), next_pos());

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
    apple.x = std::rand() % bounds.x;
    apple.y = std::rand() % bounds.y;

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
      vec_t next_pos = snake.next_pos();


      // check if the snakes head is on the apple, if so make longer and
      // move the apple
      if(next_pos.x == apple.x && next_pos.y == apple.y) {
        snake.length++;
        place_apple();
      }

      // check if the snakes head has collided with the walls
      if(next_pos.x < 0 || next_pos.x >= bounds.x ||
         next_pos.y < 0 || next_pos.y >= bounds.y) {
        state = GAME_OVER;
      }

      // check if the snakes head has collided with it's tail
      for(uint32_t i = 1; i < snake.body.size() - 1; i++) {
        vec_t b = snake.body[i];
        if(b.x == next_pos.x && b.y == next_pos.y) {
          state = GAME_OVER;
        }
      }

      if(state != GAME_OVER) {
        snake.move();
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
void tile(color_t col, int32_t x, int32_t y, int d = 0) {
  pen(col);
  frect(x * scale + d, y * scale + d, scale - d * 2, scale - d * 2);
}

vec_t t(vec_t v) {
  v.x = (v.x * scale) + 6;
  v.y = (v.y * scale) + 18;
  return v;
}

void draw() {
  // clear the screen
  pen(10, 12, 0);
  clear();

  pen(0, 0, 0, 4);
  hline(2, 12, 116);
  text(str(snake.length - 1), 2, 3);


  text(str(snake.head().x) + ", " + str(snake.head().y), 60, 3);

  // draw the walls
  rect(2, 14, 116, 104);

  // draw the apple
  fcircle(t(apple).x + 3, t(apple).y + 3, 2);

  // draw the snake
  for(uint32_t i = 0; i < snake.body.size(); i++) {
    vec_t p = t(snake.body[i]);
    frect(p.x, p.y, scale, scale);
  }

  for(int i = 0; i < 10; i++) {
    fcircle(i * 20 + 10, 40, i);
  }

  // if pause then overlay a message
  if(state == PAUSED) {
    pen(4, 4, 4, 8);
    rect(0, 0, 120, 120);

    pen(15, 15, 15);
    text("PAUSED", 10, 10);
  }

  if(state == GAME_OVER) {
   /* pen(15, 0, 0, 8);
    rect(0, 0, 120, 120);*/

    pen(15, 15, 15);
    text("GAME OVER", 10, 10);
    text("YOU SCORED", 10, 20);
    text(str(snake.length), 10, 30);

    text("PRESS A TO", 10, 50);
    text(" RESTART  ", 10, 60);
  }
}