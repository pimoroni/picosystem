#include "picosystem.hpp"

namespace picosystem {

  // drawing
  void clear() {
    rect(_cx, _cy, _cw, _ch);
  }

  void pixel(int32_t x, int32_t y) {
    _camera_offset(x, y);
    if(contains(x, y, _cx, _cy, _cw, _ch)) {
      _bf(&_pen, 0, _dt.p(x, y), 1);
    }
  }

  void hspan(int32_t x, int32_t y, int32_t c) {
    _camera_offset(x, y);
    if(y < _cy || y >= _cy + _ch) {return;}
    if(x < _cx) {c -= (_cx - x); x = _cx;}
    if(x + c >= _cx + _cw) {c -= (x + c) - (_cx + _cw);}
    if(c > 0) {
      _bf(&_pen, 0, _dt.p(x, y), c);
    }
  }

  void vspan(int32_t x, int32_t y, int32_t c) {
    _camera_offset(x, y);
    if(x < _cx || x >= _cx + _cw) {return;}
    if(y < _cy) {c -= (_cy - y); y = _cy;}
    if(y + c >= _cy + _ch) {c -= (y + c) - (_cy + _ch);}
    color_t *dest = _dt.p(x, y);
    while(c-- > 0) {
      _bf(&_pen, 0, dest, 1);
      dest += _dt.w;
    }
  }

  void rect(int32_t x, int32_t y, int32_t w, int32_t h) {
    _camera_offset(x, y);
    intersection(x, y, w, h, _cx, _cy, _cw, _ch);
    color_t *dest = _dt.p(x, y);
    while(h--) {
      _bf(&_pen, 0, dest, w);
      dest += _dt.w;
    }
  }

  void circle(int32_t x, int32_t y, int32_t r) {
    _camera_offset(x, y);
    if(!intersects(x - r, y - r, r + r, r + r, _cx, _cy, _cw, _ch)) {
      return;
    }

    int ox = r, oy = 0, err = -r;
    while (ox >= oy)
    {
      int last_oy = oy; err += oy; oy++; err += oy;
      hspan(x - ox, y + last_oy, ox * 2 + 1);
      if (last_oy != 0) {hspan(x - ox, y - last_oy, ox * 2 + 1);}
      if(err >= 0 && ox != last_oy) {
        hspan(x - last_oy, y + ox, last_oy * 2 + 1);
        if (ox != 0) {hspan(x - last_oy, y - ox, last_oy * 2 + 1);}
        err -= ox; ox--; err -= ox;
      }
    }
  }

  void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    int32_t	x = x1, y = y1, dx, dy, incx, incy, balance;

    if(x2 >= x1) {dx = x2 - x1; incx = 1;} else {dx = x1 - x2; incx = -1;}
    if(y2 >= y1) {dy = y2 - y1; incy = 1;} else {dy = y1 - y2; incy = -1;}

    if(dx >= dy) {
      dy <<= 1; balance = dy - dx; dx <<= 1;
      while(x != x2) {
        pixel(x, y);
        if (balance >= 0) {y += incy; balance -= dx;}
        balance += dy; x += incx;
      }
    } else {
      dx <<= 1; balance = dx - dy; dy <<= 1;
      while(y != y2) {
        pixel(x, y);
        if(balance >= 0) {x += incx; balance -= dy;}
        balance += dx; y += incy;
      }
    }
  }

  void blit(const buffer_t &src, int32_t x, int32_t y, int32_t w, int32_t h, int32_t dx, int32_t dy) {
    _camera_offset(x, y);
    intersection(x, y, w, h, _cx, _cy, _cw, _ch);

    color_t *ps = src.data + (x + y * src.w);
    color_t *pd = _dt.data + (dx + dy * _dt.w);

    while(h--) {
      _bf(ps, 1, pd, w); // draw row
      pd += _dt.w;
      ps += src.w;
    }
  }

  void sprite(uint32_t i, int32_t x, int32_t y, uint8_t flags) {
    uint32_t sx = i % (_ss->w / 8);
    uint32_t sy = i / (_ss->w / 8);

    blit(*_ss, sx * 8, sy * 8, 8, 8, x, y);
  }

  // draws a character at the current cursor position
  void text(const char &c) {
    if(!intersects(_tx, _ty, 8, 8, _cx, _cy, _cw, _ch)) {
      return;
    }

    const uint8_t *p = &_font[c * 8];
    color_t *dest = _dt.p(_tx, _ty);

    for(int32_t y = _ty; y < _ty + 8; y++) {
      if(y >= _cy && y < _cy + _ch) {
        uint8_t pr = *p;

        for(uint8_t x = _tx; x < _tx + 8; x++) {
          if(x >= _cx && x < _cx + _cw && pr & 1) {
            _bf(&_pen, 0, dest, 1);
          }
          pr >>= 1; dest++;
        }
      }
      p++; dest += _dt.w - 8;
    }

    _tx += 8;
  }

  void text(const char &c, int32_t x, int32_t y) {
    _camera_offset(x, y);

    _tx = x;
    _ty = y;

    text(c);
  }

  void text(const std::string &t, int32_t x, int32_t y) {
    _camera_offset(x, y);

    _tx = x;
    _ty = y;

    text(t);
  }

  void text(const std::string &t) {
    int32_t _stx = _tx;

    for(auto &c : t) {
      // newline
      if(c == 10) {
        // set cursor to start of next line
        _ty += 8;
        _tx = _stx;
      } else {
        text(c);
      }
    }

    // set cursor to start of next line
    _ty += 8;
    _tx = _stx;
  }

  void _logo() {
    uint8_t *s = _picosystem_logo;

    for(int y = 35; y < 85; y++) {
      for(int x = 19; x < 101; x+=8) {
        for(int bit = 0; bit < 8; bit++) {
          if(*s & (0b10000000 >> bit)) {
            pixel(x + bit, y);
          }
        }
        s++;
      }
    }
  }

}