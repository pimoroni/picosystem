#include "picosystem.hpp"

using namespace picosystem;

// helpful vertex/vector type with operators and a few key functions
struct v2_t {
  float x, y;

  v2_t() : x(0), y(0) {}
  v2_t(float x, float y) : x(x), y(y) {}

  float length() { return sqrtf(x * x + y * y); }
  void normalise() { float l = length(); x /= l; y /= l; }
  void rotate(float a) {
    float r = deg2rad(a), s = sin(r), c = cos(r);
    float t = c * x - s * y;
    y = s * x + c * y;
    x = t;
  }
  float angle() {
    float a = rad2deg(atan2(x, -y));
    return a < 0.0f ? a + 360.0f : a;
  }

  // operators
  v2_t& operator+=(const v2_t& rhs) { x += rhs.x; y += rhs.y; return *this; }
  v2_t& operator-=(const v2_t& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
  v2_t& operator*=(const v2_t& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
  v2_t& operator/=(const v2_t& rhs) { x /= rhs.x; y /= rhs.y; return *this; }

  friend v2_t operator+(v2_t lhs, const v2_t& rhs) { lhs += rhs; return lhs; }
  friend v2_t operator-(v2_t lhs, const v2_t& rhs) { lhs -= rhs; return lhs; }
  friend v2_t operator*(v2_t lhs, const v2_t& rhs) { lhs *= rhs; return lhs; }
  friend v2_t operator/(v2_t lhs, const v2_t& rhs) { lhs /= rhs; return lhs; }

  v2_t& operator+=(const float& rhs) { x += rhs; y += rhs; return *this; }
  v2_t& operator-=(const float& rhs) { x -= rhs; y -= rhs; return *this; }
  v2_t& operator*=(const float& rhs) { x *= rhs; y *= rhs; return *this; }
  v2_t& operator/=(const float& rhs) { x /= rhs; y /= rhs; return *this; }

  friend v2_t operator+(v2_t lhs, const float& rhs) { lhs += rhs; return lhs; }
  friend v2_t operator-(v2_t lhs, const float& rhs) { lhs -= rhs; return lhs; }
  friend v2_t operator*(v2_t lhs, const float& rhs) { lhs *= rhs; return lhs; }
  friend v2_t operator/(v2_t lhs, const float& rhs) { lhs /= rhs; return lhs; }
};

// contain all of the details about a single particle
struct particle_t {
  template <class T>
  friend struct emitter_t;

  // properties
  v2_t pos;                     // position vertex
  v2_t dir;                     // direction vector
  float age;                    // value from 0..1 representing stage of life

  // internal use
  protected:
  int32_t life_ms;              // counts down to zero
  particle_t *next = nullptr;   // next particle in list

  public:
  void destroy() {life_ms = 0;}
  void create() {}
  void update(float elapsed) {}
  void draw() {}
};

template <class T>
struct emitter_t;

// emitters create, update, and destroy particles as needed
template <class T>
struct emitter_t {
  // emitter properties
  uint32_t max_count;           // max particle count
  uint32_t count = 0;           // current particle count
  int32_t max_life_ms;          // max lifetime for particles
  int32_t last_ms = -1;

  // it's slightly more complicated but we create a pool of particles to use
  // over and over as it avoids memory fragmentation caused by allocating
  // particles dynamically as needed and is faster than using a container
  // like std::vector where we would need to erase elements in the middle
  T *unused = nullptr; // pool of unused particles
  T *active = nullptr; // poll of active particle

  // constructor
  emitter_t(
    uint32_t max_count, uint32_t max_life_ms)
    : max_count(max_count), max_life_ms(max_life_ms) {
    // initialise unused particle pool
    unused = new T[max_count];
    for(uint32_t i = 0; i < max_count - 1; i++) {
      unused[i].next = &unused[i + 1];
    }

    last_ms = time();
  }

  // return the number of particles in the unused pool
  uint32_t unused_count() {
    return max_count - count;
  }

  // reset emitter state
  void reset() {
    // return all particles back to the unused pool
    while(active) {
      T *pn = (T *)active->next;
      active->next = unused;
      unused = active;
      active = pn;
    }
    count = 0;
    // reset first timing
    last_ms = time();
  }

  // create a new particle
  void create() {
    // pop first unused particle
    T *p = unused;
    unused = p ? (T *)p->next : nullptr;

    if(count > max_count) {
        return;
    }

    if(p) { // if we got one...
      // replace as head of active list
      p->next = active;
      active = p;
      // set initial age
      active->life_ms = max_life_ms;
      active->age = 0.0f;
      // call user create function to set initial state
      p->create();
      count++;
    }
  }

  void destroy(T *p) {
    if(p == active) {
      // special case if head of list
      active = (T *)p->next;
      count--;
    }else{
      // find the previous sibling in the list and remove particle
      T *test = active;
      while(test) {
        if(test->next == p) {
          // splice it out
          test->next = p->next;
        }
        test = (T *)test->next;
      }
      count--;
    }

    // replace as head of unused list
    p->next = unused;
    unused = p;
  }

  void update() {
    int32_t current_ms = time();
    int32_t elapsed_ms = current_ms - last_ms;

    T *p = active;
    while(p) {
      // next pointer invalid if we destory so save it first
      T *n = (T *)p->next;

      p->life_ms -= elapsed_ms;

      if(p->life_ms <= 0) {
        // destroy if life ran out
        destroy(p);

        // create a new particle to replace the destroyed one
        create();
      }else{
        // update age
        p->age = float(max_life_ms - p->life_ms) / float(max_life_ms);

        // call update function
        float elapsed = elapsed_ms / 1000.0f;
        p->update(elapsed);
      }

      p = n;
      last_ms = current_ms;
    }

    // calculate the rate of creation of new particles based on max life,
    // this ensures that we build up to the max_count gradually instead of
    // blasting them all out at once
    if(unused) {
      static float c = 0.0f;
      c += (max_count * elapsed_ms / float(max_life_ms));
      while(c > 1.0f) {
        create();
        c--;
      }
    }
  }

  void draw() {
    T *p = active;
    while(p) {
      p->draw();
      p = (T *)p->next;
    }
  }
};