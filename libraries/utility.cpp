#include <math.h>

#include "picosystem.hpp"

namespace picosystem {

  float _fsin_lut[256];
  constexpr float _2PI_LUTS = 256.0f / (_PI * 2.0f);

  std::string str(float v, uint8_t precision) {
    static char b[32];
    snprintf(b, 32, "%.*f", precision, v);
    return b;
  }

  std::string str(int32_t v) {
    static char b[32];
    snprintf(b, 32, "%ld", v);
    return b;
  }

  std::string str(std::size_t v) {
    return str(int32_t(v));
  }

  std::string str(uint32_t v) {
    static char b[32];
    snprintf(b, 32, "%lu", v);
    return b;
  }

  color_t rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // color_t will contain pixel data in the format aaaarrrrggggbbbb
    return (r & 0xf) | ((a & 0xf) << 4) | ((b & 0xf) << 8) | ((g & 0xf) << 12);
  }

   color_t hsv(float h, float s, float v, float a) {
    h = h - floor(h); // clamp to 0..1
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 15.0f;
    a *= 15.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: return rgb(v, t, p, a); break;
      case 1: return rgb(q, v, p, a); break;
      case 2: return rgb(p, v, t, a); break;
      case 3: return rgb(p, q, v, a); break;
      case 4: return rgb(t, p, v, a); break;
      case 5: return rgb(v, p, q, a); break;
    }

    return rgb(0, 0, 0);
  }

  buffer_t* buffer(uint32_t w, uint32_t h, void *data) {
    buffer_t *b = new buffer_t();
    b->w = w;
    b->h = h;
    if (data)
    {
      b->data = (color_t *)data;
      b->alloc = false;
    }
    else
    {
      b->data = new color_t[w * h]{};
      b->alloc = true;
    }
    return b;
  }

  // returns true if the provided rectangles intersect
  bool intersects(
    int32_t x, int32_t y, int32_t w, int32_t h,
    int32_t cx, int32_t cy, int32_t cw, int32_t ch) {
    return !(x > cx + cw || x + w < cx || y > cy + ch || y + h < cy);
  }

  // updates x, y, w, and h to be the intersection of the two provided rectangles
  void intersection(
    int32_t &x, int32_t &y, int32_t &w, int32_t &h,
    int32_t cx, int32_t cy, int32_t cw, int32_t ch) {
    w = std::max(0L, (long int)std::min(x + w, cx + cw) - std::max(x, cx));
    h = std::max(0L, (long int)std::min(y + h, cy + ch) - std::max(y, cy));
    x = std::max(x, cx);
    y = std::max(y, cy);
  }

  // returns true if the point is inside the provided rectangle
  bool contains(int32_t x, int32_t y,
    int32_t cx, int32_t cy, int32_t cw, int32_t ch) {
    return x >= cx && y >= cy && x < cx + cw && y < cy + ch;
  }

  // returns true if the rect is inside the provided rectangle
  bool contains(int32_t x, int32_t y, int32_t w, int32_t h,
    int32_t cx, int32_t cy, int32_t cw, int32_t ch) {
    return x >= cx && y >= cy && x + w <= cx + cw && y + h <= cy + ch;
  }

  std::vector<std::string> split(const std::string& t, char d) {
      std::vector<std::string> l;
      size_t start, end = 0;
      while ((start = t.find_first_not_of(d, end)) != std::string::npos) {
        end = t.find(d, start);
        l.push_back(t.substr(start, end - start));
      }
      return l;
  }

  float fsin(float v) {
    return _fsin_lut[uint8_t(v * _2PI_LUTS)];
  }

  float fcos(float v) {
    return _fsin_lut[uint8_t((v * _2PI_LUTS) + 64)];
  }

}
