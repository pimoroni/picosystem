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
    snprintf(b, 32, "%d", v);
    return b;
  }

  std::string str(uint32_t v) {
    static char b[32];
    snprintf(b, 32, "%d", v);
    return b;
  }

  color_t rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // color_t will contain pixel data in the format aaaarrrrggggbbbb
    return (r & 0xf) | ((a & 0xf) << 4) | ((b & 0xf) << 8) | ((g & 0xf) << 12);
  }

  color_t hsv(float h, float s, float v, float a) {
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

  uint32_t text_width(std::string &t) {
    // add up length of characters
    uint32_t l = 0;
    for(auto c : t) {
      l += _font[(c - 32) * 9] + 1;
    }
    return l;
  }

  uint32_t _word_length(std::string &t, std::size_t &i) {
    // skip past any spaces if present
    i = t.find_first_not_of(' ', i);

    if(i == std::string::npos) {
      return std::string::npos;
    }

    // find next space after current word
    std::size_t n = t.find(' ', i);

    if(n == std::string::npos) {
      n = t.length();
    }

    // add up length of characters
    uint32_t l = 0;
    while(i < n) {
      l += _font[(t[i] - 32) * 9] + 1;
      i++;
    };

    return l;
  }

  void wrap(std::string &t, std::size_t w) {
    std::size_t i = 0, si = 0, ll = 0;

    while(true) {
      uint32_t wl = _word_length(t, i);

      if(i == std::string::npos) {
        break;
      }

      if(ll + wl >= w) {
        // if we would overflow the line then replace space with a newline
        t[si] = '\n';
        // next line starts with current word
        ll = wl + 2;
      }else{
        ll += wl + 2; // 2 == space length, should be a configurable?
      }

      si = i;
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