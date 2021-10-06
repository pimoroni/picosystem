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

  std::string str(uint32_t v) {
    static char b[32];
    snprintf(b, 32, "%d", v);
    return b;
  }

  color_t color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // color_t will contain pixel data in the format aaaarrrrggggbbbb
    return (r & 0xf) | ((a & 0xf) << 4) | ((b & 0xf) << 8) | ((g & 0xf) << 12);
  }

  color_t hsv_color(float h, float s, float v, float a) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 15.0f;
    a *= 15.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: return color(v, t, p, a); break;
      case 1: return color(q, v, p, a); break;
      case 2: return color(p, v, t, a); break;
      case 3: return color(p, q, v, a); break;
      case 4: return color(t, p, v, a); break;
      case 5: return color(v, p, q, a); break;
    }

    return color(0, 0, 0);
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

  void wrap(std::string &t, std::size_t chars) {
    std::size_t i = 0, j = 0;

    while(true) {
      std::size_t k = t.find(' ', j + 1);

      if(k == std::string::npos) {
        // no more spaces found, exit
        break;
      } else if(k < i + chars) {
        // space found still fits in word wrap, update j
        j = k;
      } else {
        // space found beyond word wrap, replace with newline
        // at j and continue searching
        t[j] = '\n';
        i = j + 1;
      }
    }
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