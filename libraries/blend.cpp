#include <string.h>

#include "picosystem.hpp"

namespace picosystem {

  // copy the source over the destination, ignoring alpha channel
  void COPY(color_t *ps, uint32_t so, int32_t ss, color_t *pd, uint32_t c) {
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
  void MASK(color_t *ps, uint32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      color_t s = *(ps + (so >> 16));

      // copy if alpha component
      if(s & 0x00f0) { *pd = s; }

      // step destination
      pd++;

      // step source
      so += ss;
    }
  }

  // blends the source and destination
  void BLEND(color_t *ps, uint32_t so, int32_t ss, color_t *pd, uint32_t c) {
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