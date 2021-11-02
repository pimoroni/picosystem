#include <cassert>

#include "picosystem.hpp"

namespace picosystem {

  // drawing
  void clear() {
    frect(_cx, _cy, _cw, _ch);
  }

  void pixel(int32_t x, int32_t y) {
    _camera_offset(x, y);
    if(contains(x, y, _cx, _cy, _cw, _ch)) {
      _bf(&_pen, 0, 0, _dt->p(x, y), 1);
    }
  }

  void hline(int32_t x, int32_t y, int32_t c) {
    _camera_offset(x, y);
    if(y < _cy || y >= _cy + _ch) {return;}
    if(x < _cx) {c -= (_cx - x); x = _cx;}
    if(x + c >= _cx + _cw) {c -= (x + c) - (_cx + _cw);}
    if(c > 0) {
      _bf(&_pen, 0, 0, _dt->p(x, y), c);
    }
  }

  void vline(int32_t x, int32_t y, int32_t c) {
    _camera_offset(x, y);
    if(x < _cx || x >= _cx + _cw) {return;}
    if(y < _cy) {c -= (_cy - y); y = _cy;}
    if(y + c >= _cy + _ch) {c -= (y + c) - (_cy + _ch);}
    color_t *dest = _dt->p(x, y);
    while(c-- > 0) {
      _bf(&_pen, 0, 0, dest, 1);
      dest += _dt->w;
    }
  }

  void rect(int32_t x, int32_t y, int32_t w, int32_t h) {
    _camera_offset(x, y);
    _camera_offset(false);
    hline(x, y, w);
    vline(x, y + 1, h - 1);
    hline(x + 1, y + h - 1, w - 1);
    vline(x + w - 1, y, h - 1);
    _camera_offset(true);
  }

  void frect(int32_t x, int32_t y, int32_t w, int32_t h) {
    _camera_offset(x, y);
    intersection(x, y, w, h, _cx, _cy, _cw, _ch);
    color_t *dest = _dt->p(x, y);
    while(h--) {
      _bf(&_pen, 0, 0, dest, w);
      dest += _dt->w;
    }
  }

  void circle(int32_t x, int32_t y, int32_t r) {
    _camera_offset(x, y);
    if(!intersects(x - r, y - r, r + r, r + r, _cx, _cy, _cw, _ch)) {
      return;
    }

    _camera_offset(false);

    int32_t p = 3 - 2 * r;
    int32_t xc = 0, yc = r;

    pixel(x + xc, y + yc);
    pixel(x + xc, y - yc);
    pixel(x + yc, y + xc);
    pixel(x - yc, y + xc);

    while(xc < yc)
    {
      if(p <= 0) {
        p = p + (4 * xc) + 6;
        xc++;
      } else {
        p = p + (4 * (xc - yc)) + 10;
        xc++; yc--;
      }

      pixel(x + xc, y + yc);
      pixel(x - xc, y + yc);
      pixel(x + xc, y - yc);
      pixel(x - xc, y - yc);
      pixel(x + yc, y + xc);
      pixel(x - yc, y + xc);
      pixel(x + yc, y - xc);
      pixel(x - yc, y - xc);
    }
    _camera_offset(true);
  }

  void fcircle(int32_t x, int32_t y, int32_t r) {
    _camera_offset(x, y);
    if(!intersects(x - r, y - r, r + r, r + r, _cx, _cy, _cw, _ch)) {
      return;
    }

    _camera_offset(false);

    int ox = r, oy = 0, err = -r;
    while (ox >= oy)
    {
      int last_oy = oy; err += oy; oy++; err += oy;
      hline(x - ox, y + last_oy, ox * 2 + 1);
      if (last_oy != 0) {hline(x - ox, y - last_oy, ox * 2 + 1);}
      if(err >= 0 && ox != last_oy) {
        hline(x - last_oy, y + ox, last_oy * 2 + 1);
        if (ox != 0) {hline(x - last_oy, y - ox, last_oy * 2 + 1);}
        err -= ox; ox--; err -= ox;
      }
    }
    _camera_offset(true);
  }

  void ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry) {
    _camera_offset(x, y);
    if(!intersects(x - rx, y - ry, rx + rx, ry + ry, _cx, _cy, _cw, _ch)) {
      return;
    }

    if(rx <= 0 || ry <= 0) {
      return;
    }

    _camera_offset(false);

    int ryy = ry * ry;
    int rxx = rx * rx;
    int ryyxx = ryy * rxx;
    int x0 = rx;
    int dx = 0;

    pixel(x + -rx, y);
    pixel(x + rx - 1, y);

    for (uint32_t py = 1; py <= ry; py++) {
      int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
      for( ; x1 > 0; x1--) {
        if(x1 * x1 * ryy + py * py * rxx <= ryyxx)
          break;
      }
      dx = x0 - x1;  // current approximation of the slope
      x0 = x1;

      int dxw = std::max(1, dx);
      // upper
      hline(x + -x0 - dx, y - py, dxw);
      hline(x + x0, y - py, dxw);

      // lower
      hline(x + -x0 - dx, y + py, dxw);
      hline(x + x0, y + py, dxw);
    }

    _camera_offset(true);
  }

  void fellipse(int32_t x, int32_t y, int32_t rx, int32_t ry) {
    _camera_offset(x, y);
    if(!intersects(x - rx, y - ry, rx + rx, ry + ry, _cx, _cy, _cw, _ch)) {
      return;
    }

    if(rx <= 0 || ry <= 0) {
      return;
    }

    _camera_offset(false);

    int ryy = ry * ry;
    int rxx = rx * rx;
    int ryyxx = ryy * rxx;
    int x0 = rx;
    int dx = 0;

    hline(x + -rx, y, rx + rx);

    for (uint32_t py = 1; py <= ry; py++) {
      int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
      for( ; x1 > 0; x1--) {
        if(x1 * x1 * ryy + py * py * rxx <= ryyxx)
          break;
      }
      dx = x0 - x1;  // current approximation of the slope
      x0 = x1;

      hline(x + -x0, y - py, x0 * 2);
      hline(x + -x0, y + py, x0 * 2);
    }

    _camera_offset(true);
  }

  void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    _camera_offset(x1, y1);
    _camera_offset(x2, y2);
    _camera_offset(false);
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
    _camera_offset(true);
  }

  void fpoly(const int32_t *p, uint32_t l) {
    int32_t nodes[64]; // max number of nodes per scanline

    if(l <= 1) {
      return;
    }

    int32_t miny = p[1], maxy = p[1];

    for (uint16_t i = 1; i < l; i++) {
      miny = std::min(miny, p[i * 2 + 1]);
      maxy = std::max(maxy, p[i * 2 + 1]);
    }

    // for each scanline within the polygon bounds (clipped to clip rect)
    int32_t y;

    for (y = std::max(_cy, miny); y <= std::min(_cy + _ch, maxy); y++) {
      uint8_t n = 0;
      for (uint16_t i = 0; i < l; i++) {
        uint16_t j = (i + 1) % l;
        int32_t sy = p[i * 2 + 1];
        int32_t ey = p[j * 2 + 1];
        int32_t fy = y;
        if ((sy < fy && ey >= fy) || (ey < fy && sy >= fy)) {
          int32_t sx = p[i * 2];
          int32_t ex = p[j * 2];
          int32_t px = int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx));

          nodes[n++] = px < _cx ? _cx : (px >= _cx + _cw ? _cx + _cw - 1 : px);
        }
      }

      uint16_t i = 0;
      while (i < n - 1) {
        if (nodes[i] > nodes[i + 1]) {
          int32_t s = nodes[i]; nodes[i] = nodes[i + 1]; nodes[i + 1] = s;
          if (i) i--;
        }
        else {
          i++;
        }
      }

      for (i = 0; i < n; i += 2) {
        hline(nodes[i], y, nodes[i + 1] - nodes[i] + 1);
      }
    }
  }

  void fpoly(const std::initializer_list<int32_t> &pts) {
    fpoly(pts.begin(), pts.size() / 2);
  }

  void poly(const int32_t *p, uint32_t l) {
    for(uint32_t i = 0; i < l - 1; i++) {
      line(p[i * 2], p[i * 2 + 1], p[i * 2 + 2], p[i * 2 + 3]);
    }
    line(p[l * 2 - 2], p[l * 2 - 1], p[0], p[1]);
  }

  void poly(const std::initializer_list<int32_t> &pts) {
    poly(pts.begin(), pts.size() / 2);
  }

  void blit(buffer_t *src, int32_t sx, int32_t sy, int32_t w, int32_t h, int32_t dx, int32_t dy, uint32_t flags) {
    _camera_offset(dx, dy);

    if(!intersects(dx, dy, w, h, _cx, _cy, _cw, _ch)) {
      return;
    }

    // clip source coordinates
    if(sx < 0) {dx += -sx; w += sx; sx = 0;}
    if(sy < 0) {dy += -sy; h += sy; sy = 0;}
    if(sx + w >= (int32_t)src->w) {w -= (sx + w) - src->w;}
    if(sy + h >= (int32_t)src->h) {h -= (sy + h) - src->h;}

    // clip destination coordinates
    if(dx < _cx) {sx += (_cx - dx); w += (_cx - dx); dx = _cx;}
    if(dy < _cy) {sy += (_cy - dy); h += (_cy - dy); dy = _cy;}
    if(dx + w >= _cx + _cw) {w -= (dx + w) - (_cx + _cw);}
    if(dy + h >= _cy + _ch) {h -= (dy + h) - (_cy + _ch);}

    if(w <= 0 || h <= 0) {
      return;
    }

    color_t *ps = src->data + (sx + sy * src->w);
    color_t *pd = _dt->data + (dx + dy * _dt->w);

    int32_t so = 0;
    int32_t ss = 1 << 16;

    if(flags & HFLIP) {
      ss = (-1) << 16;
      so = (w - sx - 1) << 16;
    }

    int32_t ds = _dt->w;

    if(flags & VFLIP) {
      pd += (h - 1) * _dt->w;
      ds = -_dt->w;
    }

    while(h--) {
      _bf(ps, so, ss, pd, w); // draw row
      pd += ds;
      ps += src->w;
    }
  }

  void blit(buffer_t *src, int32_t sx, int32_t sy, int32_t sw, int32_t sh, int32_t dx, int32_t dy, int32_t dw, int32_t dh, uint32_t flags) {
    _camera_offset(dx, dy);

    if(!intersects(dx, dy, dw, dh, _cx, _cy, _cw, _ch)) {
      return;
    }

    // source sample x/y and step
    int32_t ssy = 0, ssys = (sh << 16) / dh;
    int32_t ssx = 0, ssxs = (sw << 16) / dw;

    color_t *pd = _dt->p(dx, dy);
    color_t *ps = src->p(sx, sy);

    // if we need to offset our start to the clip area then we need to jump
    // ahead in the source
    if(dy < _cy) {
      ssy = ssys * (_cy - dy);
      pd += _dt->w * (_cy - dy); ps += src->w * (ssy >> 16);
      ssy &= 0xffff;
      dh -= (_cy - dy); dy = _cy;
    }
    int32_t maxy = std::min(dy + dh, _cy + _ch);

    if(dx < _cx) {
      ssx += ssxs * (_cx - dx); pd += (_cx - dx);
      dw -= (_cx - dx); dx = _cx;
    }
    int32_t w = std::min(dw, _cw);

    if(flags & HFLIP) {
      ssx += (w - 1) * ssxs;
      ssxs = -ssxs;
    }

    int32_t ds = _dt->w;

    if(flags & VFLIP) {
      pd += (maxy - dy - 1) * _dt->w;
      ds = -_dt->w;
    }

    // loop for all visible scanlines
    for(int32_t y = dy; y < maxy; y++) {
      _bf(ps, ssx, ssxs, pd, w);

      ssy += ssys;
      pd += ds;
      ps += src->w * (ssy >> 16);
      ssy &= 0xffff;
    }
  }

  void sprite(uint32_t i, int32_t x, int32_t y) {
    uint32_t sx = i % (_ss->w / 8);
    uint32_t sy = i / (_ss->w / 8);

    blit(_ss, sx * 8, sy * 8, 8, 8, x, y);
  }

  void sprite(uint32_t i, int32_t x, int32_t y, int32_t cx, int32_t cy) {
    uint32_t sx = i % (_ss->w / 8);
    uint32_t sy = i / (_ss->w / 8);

    blit(_ss, sx * 8, sy * 8, cx * 8, cy * 8, x, y);
  }

  void sprite(
    uint32_t i,
    int32_t x, int32_t y, int32_t cx, int32_t cy,
    int32_t dw, int32_t dh) {
    uint32_t sx = i % (_ss->w / 8);
    uint32_t sy = i / (_ss->w / 8);
    blit(_ss, sx * 8, sy * 8, cx * 8, cy * 8, x, y, dw, dh);
  }

  void sprite(
    uint32_t i,
    int32_t x, int32_t y, int32_t cx, int32_t cy,
    int32_t dw, int32_t dh, uint32_t flags) {
    uint32_t sx = i % (_ss->w / 8);
    uint32_t sy = i / (_ss->w / 8);
    blit(_ss, sx * 8, sy * 8, cx * 8, cy * 8, x, y, dw, dh, flags);
  }


  void _logo() {
    const uint8_t *s = _picosystem_logo;

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
