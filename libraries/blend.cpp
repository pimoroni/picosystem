#include <string.h>

#include "picosystem.hpp"

namespace picosystem {

  // helper functions for manipulating the channels of our pixel format

  inline __attribute__((always_inline)) uint8_t _extract_alpha(uint32_t s) {
    // grab alpha channel from source
    uint8_t alpha = (s & 0xf0000) >> 16;

    // combine source alpha with global alpha (adding 0xf makes operation act
    // as ceil() instead of floor()
    alpha = ((alpha * _a) + 0xf) >> 4;

    // stretch alpha to 0..16 from 0..15 to allow us to use >>4 later in the
    // blend for much better performance than /15 - we do this by adding the
    // most significant bit meaning that possible values go 0...7 and 9..16
    // skipping the middle bit
    alpha += ((alpha & 0b1000) >> 3);

    return alpha;
  }

  inline __attribute__((always_inline)) uint32_t _blend(uint32_t s, uint32_t d, uint8_t a) {
    // blend source and destination (adding 0xf to each channel to makes
    // end result ceil() instead of floor())
    return ((s * a + d * (0x10 - a)) + 0x0f0f0f0f) >> 4;
  }

  inline __attribute__((always_inline)) uint32_t _unpack_channels(uint32_t s) {
    // unpack source into 32 bits with space for alpha multiplication
    // we start with (----GBAR) and end up with (-G-A-B-R)
    return (s | ((s & 0xf0f0) << 12)) & 0x0f0f0f0f;
  }

  inline __attribute__((always_inline)) uint32_t _pack_channels(uint32_t s) {
    // reconstruct 16bit destination colour
    return (s & 0x0f0f) | ((s >> 12) & 0xf0f0);
  }

  color_t mix(color_t c1, color_t c2, uint8_t m) {
    m += ((m & 0b1000) >> 3);
    uint32_t s = _unpack_channels(c1);
    uint32_t d = _unpack_channels(c2);
    return _pack_channels(_blend(s, d, m));
  }

  // blend mode functions

   // blends the source and destination
  void ALPHA(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    uint32_t s = _unpack_channels(*(ps + (so >> 16)));
    uint8_t alpha = _extract_alpha(s);

    while(c--) {
      uint32_t d = _unpack_channels(*pd);
      d = _blend(s, d, alpha);
      *pd++ = _pack_channels(d);

      // step source if needed
      if(ss) {
        so += ss;
        s = _unpack_channels(*(ps + (so >> 16)));
        alpha = _extract_alpha(s);
      }
    }
  }

  // copy the source over the destination, ignoring alpha channel
  void COPY(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    if(!ss) {
      // optimised for no source step
      if(uintptr_t(pd) & 0b11 && c) { *pd++ = *ps; c--; } // align to 32bits
      uint32_t *pda = (uint32_t *)pd; uint32_t s = *ps | (*ps << 16);
      while(c > 1) { *pda++ = s; c-=2; } // two pixel copy
      if(c) { pd = (color_t *)pda; *pd = *ps; } // finish if needed
    }else if(ss == 1 << 16) {
      // optimised for source step == 1
      memcpy(pd, ps, c * 2);
    }else{
      // otherwise...
      while(c--) { *pd++ = *(ps + (so >> 16)); so += ss; }
    }
  }

  // copy the source over the destination if source alpha is != 0
  // allows for faster blitting of sprites that only need 1-bit alpha
  void MASK(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));

      // copy if alpha component
      if(s & 0x00f0) { *pd = s; }

      // step destination and source
      pd++; so += ss;
    }
  }

  // uses ALPHA blend but forces pen (even if we're doing a blit) which can
  // be useful for masking sprites etc
  void PEN(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    color_t p = _pen;
    uint8_t pa = (p & 0x00f0) >> 4;

    while(c--) {
      color_t s = *(ps + (so >> 16));

      // multiply source alpha with pen
      uint8_t sa = (s & 0x00f0) >> 4;
      uint8_t a = ((pa * sa) + 0xf) >> 4;

      // replace pen alpha
      p &= 0xff0f;
      p |= a << 4;

      // blend pixel
      ALPHA(&p, 0, 0, pd, 1);

      // step destination and source
      pd++; so += ss;
    }
  }

  // compares source and destination RGB channels and picks the darker of the
  // two. if there is global alpha the result is then blended with the
  // destination.
  void DARKEN(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));
      color_t d = *pd;

      uint16_t sr = (s & 0x000f), sg = (s & 0xf000), sb = (s & 0x0f00);
      uint16_t dr = (d & 0x000f), dg = (d & 0xf000), db = (d & 0x0f00);
      uint16_t sa = (s & 0x00f0);

      s = (sr < dr ? sr : dr) | (sg < dg ? sg : dg) | (sb < db ? sb : db) | sa;
      ALPHA(&s, 0, 0, pd, 1);

      // step destination and source
      pd++; so += ss;
    }
  }

  // compares source and destination RGB channels and picks the lighter of the
  // two. if there is global alpha the result is then blended with the
  // destination.
  void LIGHTEN(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));
      color_t d = *pd;

      uint16_t sr = (s & 0x000f), sg = (s & 0xf000), sb = (s & 0x0f00);
      uint16_t dr = (d & 0x000f), dg = (d & 0xf000), db = (d & 0x0f00);
      uint16_t sa = (s & 0x00f0);

      s = (sr > dr ? sr : dr) | (sg > dg ? sg : dg) | (sb > db ? sb : db) | sa;
      ALPHA(&s, 0, 0, pd, 1);

      // step destination and source
      pd++; so += ss;
    }
  }

  // compares source and destination RGB channels and picks the lighter of the
  // two. if there is global alpha the result is then blended with the
  // destination.
  void ADD(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));
      color_t d = *pd;

      int32_t sr = (s & 0x000f), sg = (s & 0xf000), sb = (s & 0x0f00);
      int32_t dr = (d & 0x000f), dg = (d & 0xf000), db = (d & 0x0f00);
      int32_t sa = (s & 0x00f0);

      dr += sr;
      dg += sg;
      db += sb;

      dr = dr > 15 ? 15 : dr;
      dg = dg > (15 << 12) ? (15 << 12) : dg;
      db = db > (15 << 8) ? (15 << 8) : db;

      s = dr | dg | db | sa;

      ALPHA(&s, 0, 0, pd, 1);

      // step destination and source
      pd++; so += ss;
    }
  }

  // compares source and destination RGB channels and picks the lighter of the
  // two. if there is global alpha the result is then blended with the
  // destination.
  void SUBTRACT(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));
      color_t d = *pd;

      int32_t sr = (s & 0x000f), sg = (s & 0xf000), sb = (s & 0x0f00);
      int32_t dr = (d & 0x000f), dg = (d & 0xf000), db = (d & 0x0f00);
      int32_t sa = (s & 0x00f0);

      dr = dr > sr ? dr - sr : 0;
      dg = dg > sg ? dg - sg : 0;
      db = db > sb ? db - sb : 0;

      s = dr | dg | db | sa;
      ALPHA(&s, 0, 0, pd, 1);

      // step destination and source
      pd++; so += ss;
    }
  }

  // compares source and destination RGB channels and picks the lighter of the
  // two. if there is global alpha the result is then blended with the
  // destination.
  void MULTIPLY(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));
      color_t d = *pd;

      int32_t sr = (s & 0x000f), sg = (s & 0xf000), sb = (s & 0x0f00);
      int32_t dr = (d & 0x000f), dg = (d & 0xf000), db = (d & 0x0f00);
      int32_t sa = (s & 0x00f0);

      dr = (sr * dr) >> 4;
      dg = (sg * dg) >> 18;
      db = (sb * db) >> 12;

      s = dr | dg | db | sa;
      ALPHA(&s, 0, 0, pd, 1);

      // step destination and source
      pd++; so += ss;
    }
  }

  uint32_t _hash(uint32_t v) {
    int n = 4;
    do {
      v = ((v >> 8) ^ v) * 0xD2 + n;
    } while(--n);
    return v;
  }
  // performs a "fizzlefade" style effect by only copying the source pixel if
  // the destination pointer address hashes to a value < source alpha
  void DISSOLVE(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));

      int32_t sa = (s & 0x00f0) >> 4;

      // create 4-bit hash of destination pointer...
      int32_t h = _hash(int32_t(pd) >> 1) & 0x000f;
      h = h == 0 ? 0 : h - 1;
      if(h < sa) {
        COPY(&s, 0, 0, pd, 1);
      }

      // step destination and source
      pd++; so += ss;
    }
  }


}
