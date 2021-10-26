#include <string.h>

#include "picosystem.hpp"

namespace picosystem {

  // copy the source over the destination, ignoring alpha channel
  void COPY(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    if(!ss) {
      // optimised for no source step
      if(uintptr_t(pd) & 0b11 && c) { *pd++ = *ps; c--; } // align to 32bits
      uint32_t *pda = (uint32_t *)pd; uint32_t s = *ps | (*ps << 16);
      while(c > 1) { *pda++ = s; c-=2; } // two pixel copy
      if(c) { *pd++ = *ps; c--; } // finish if needed
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
    while(c--) {
      color_t s = *(ps + (so >> 16));

      // copy source alpha to pen
      p &= 0xff0f;
      p |= s & 0x00f0;

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

  // blends the source and destination
  void ALPHA(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c) {
    if(!ss) {
      // optimised for no source step

      // fetch next source pixel
      uint32_t ts = *(ps + (so >> 16));

      // extract source alpha
      uint32_t sa = (ts & 0x00f0) >> 4;

      // transparent pen, nothing to draw here...
      if(sa == 0) return;

      // add one to source alpha so that we can get away with >>4 when
      // scaling our channels back down (instead of expensive /15) this
      // is accurate enough...
      sa++;

      sa *= (_a + 1);
      sa >>= 4;

      // unpack source into 32 bits with space for alpha multiplication
      // we start with 16-bits (GBAR) and end up with 32bits (-G---B-R)
      ts = (ts | ((ts & 0xf0f0) << 12)) & 0x0f000f0f;

      // pre-multiply alpha
      ts *= sa;

      // replace alpha
      ts |= (sa << 20);

      while(c--) {
        // unpack dest into 32 bits with space for alpha multiplication
        // we start with 16-bits (GBAR) and end up with 32bits (-G-A-B-R)
        uint32_t td = *pd;
        td = (td | ((td & 0xf0f0) << 12)) & 0x0f0f0f0f;

        // blend all four channels at once
        td = (ts + td * (16 - sa)) >> 4;

        // reconstruct 16bit destination colour
        *pd++ = (td & 0x0f0f) | ((td >> 12) & 0xf0f0);
      }
    }else{
      while(c--) {
        // fetch next source pixel
        uint32_t ts = *(ps + (so >> 16));

        // extract source alpha
        uint32_t sa = (ts & 0x00f0) >> 4;

        // add one to source alpha so that we can get away with >>4 when
        // scaling our channels back down (instead of expensive /15) this
        // is accurate enough...
        if(sa != 0) sa++;

        sa *= (_a + 1);
        sa >>= 4;

        // unpack source into 32 bits with space for alpha multiplication
        // we start with 16-bits (GBAR) and end up with 32bits (-G---B-R)
        ts = (ts | ((ts & 0xf0f0) << 12)) & 0x0f000f0f;

        // pre-multiply alpha
        ts *= sa;

        // replace alpha
        ts |= (sa << 20);

        // unpack dest into 32 bits with space for alpha multiplication
        // we start with 16-bits (GBAR) and end up with 32bits (-G-A-B-R)
        uint32_t td = *pd;
        td = (td | ((td & 0xf0f0) << 12)) & 0x0f0f0f0f;

        // blend all four channels at once
        td = (ts + td * (16 - sa)) >> 4;

        // reconstruct 16bit destination colour
        *pd++ = (td & 0x0f0f) | ((td >> 12) & 0xf0f0);

        // step source
        so += ss;
      }
    }

    /*
    effectively does this but faster...

    uint16_t ts = *(ps + (so >> 16));
    uint8_t sr = (ts & 0x000f) >>  0;
    uint8_t sg = (ts & 0xf000) >> 12;
    uint8_t sb = (ts & 0x0f00) >>  8;
    uint8_t sa = (ts & 0x00f0) >>  4;

    sr *= sa;
    sg *= sa;
    sb *= sa;

    uint16_t td = *pd;
    uint8_t dr = (td & 0x000f) >>  0;
    uint8_t dg = (td & 0xf000) >> 12;
    uint8_t db = (td & 0x0f00) >>  8;
    uint8_t da = (td & 0x00f0) >>  4;

    dr = sr + dr * (15 - sa);
    dg = sg + dg * (15 - sa);
    db = sb + db * (15 - sa);
    da = sa + da * (15 - sa);

    dr /= 15;
    dg /= 15;
    db /= 15;
    da /= 15;

    *pd = (dg << 12) | (db << 8) | (da << 4) | (dr << 0);*/
  }

}