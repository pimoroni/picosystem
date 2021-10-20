#include "picosystem.hpp"

using namespace picosystem;

enum state_t {PLAYING, GAME_OVER};
state_t state = PLAYING;

// a helpful way to represent an x, y coordinate pair such as the apple
// location or the body segments of the snake
struct vec_t {
  int32_t x, y;
};

// the map will be 18x16 squares big with 6x6 pixel sized squares
constexpr vec_t bounds{.x = 18, .y = 16};
constexpr int scale = 6;

// details about the snake
struct {
  vec_t dir;
  uint32_t length;
  std::vector<vec_t> body;

  // returns the next position the snake's head will be in based on its
  // current direction
  vec_t next() {
    return {.x = body[0].x + dir.x, .y = body[0].y + dir.y};
  }

  // adds new location to front of body and if that makes the snake too long
  // then removes the last segment of its body
  void move() {
    body.insert(body.begin(), next());
    if(body.size() > length) { body.pop_back(); }
  }
} snake;

// location of the apple
vec_t apple;

// place the apple in a new random location which is not the snake location
// or part of the snakes tail
void place_apple() {
  bool hit = false;
  do {
    // generate a new location for the apple
    apple.x = std::rand() % bounds.x;
    apple.y = std::rand() % bounds.y;

    // check new apple location is not on the snake's body
    for(auto v : snake.body) {
      hit |= v.x == apple.x && v.y == apple.y;
    }
  } while(hit);
}

// convert a map coordinate into its position on the screen
vec_t transform(vec_t v) {
  return {.x = (v.x * scale) + 6, .y = (v.y * scale) + 18};
}

// initialise the world
void init() {
  // set the current state to PLAYING
  state = PLAYING;

  // place the snake back in the centre of the map, remove its tail, and
  // reset its direction
  snake.body.clear();
  snake.body.push_back({.x = 9, .y = 8});
  snake.dir = {.x = 1, .y = 0};
  snake.length = 1;

  // place the apple
  place_apple();
}

// process user input and update the world state
void update(uint32_t tick) {
  if(state == PLAYING) {
    // every 10 ticks (10 times per second) we'll update position
    if(tick % 10 == 0) {
      // player input, you can't reverse direction - for example changing
      // direction to "up" cannot occur if you're currently pointing "down"
      if(button(UP)    && snake.dir.y == 0) {snake.dir.x =  0; snake.dir.y = -1;}
      if(button(DOWN)  && snake.dir.y == 0) {snake.dir.x =  0; snake.dir.y =  1;}
      if(button(LEFT)  && snake.dir.x == 0) {snake.dir.x = -1; snake.dir.y =  0;}
      if(button(RIGHT) && snake.dir.x == 0) {snake.dir.x =  1; snake.dir.y =  0;}

      vec_t next = snake.next();

      // check if the snake's head is on the apple, if so make the snake longer
      // and move the apple to a new location
      if(next.x == apple.x && next.y == apple.y) {
        snake.length++;
        place_apple();
      }

      // check if the snake's head has collided with the walls
      if(next.x < 0 || next.x >= bounds.x ||
         next.y < 0 || next.y >= bounds.y) {
        state = GAME_OVER;
      }

      // check if the snake's head has collided with its tail
      for(uint32_t i = 1; i < snake.body.size() - 1; i++) {
        vec_t v = snake.body[i];
        if(v.x == next.x && v.y == next.y) {
          state = GAME_OVER;
        }
      }

      // if we haven't died then move the snake to its new location
      if(state != GAME_OVER) {
        snake.move();
      }
    }
  } else {
    // game over, if user presses A then restart
    if(pressed(A)) {
      init();
    }
  }
}

// draw the world
void draw(uint32_t tick) {
  // clear the screen in noxious 3310 backlight green and draw everything in
  // a faint blended black to get that cheap 90s LCD feel
  pen(10, 12, 0);
  clear();
  pen(0, 0, 0, 4);

  // draw the scoreboard
  hline(2, 12, 116);
  text(str(snake.length - 1), 3, 3);

  // draw the walls
  rect(2, 14, 116, 104);

  // draw the apple
  fcircle(transform(apple).x + 3, transform(apple).y + 3, 2);

  // draw the snake - if we're in GAME_OVER state then make it flash like in
  // the original
  bool flash = ((time() / 250) % 2) == 0;
  if(state == PLAYING || (state == GAME_OVER && flash)) {
    for(uint32_t i = 0; i < snake.body.size(); i++) {
      vec_t p = transform(snake.body[i]);
      frect(p.x, p.y, scale, scale);
    }
  }
}