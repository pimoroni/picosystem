#include <string.h>

#include "picosystem.hpp"

namespace picosystem {

  // copy the source over the destination, ignoring alpha channel
  void COPY(color_t *source, uint32_t source_step, color_t *dest, uint32_t count) {
    if(source_step) {
      // for blits (i.e. source_step == 1) we're unlikely to do much
      // better than the built in memcpy implementation!
      memcpy(dest, source, count * 2);
    }else{
      // for pen drawing (i.e. source_step == 0)

      // for longer runs of pixels we can almost double the performance
      // by copying two pixels at a time

      // align destination to 32bits
      if((uintptr_t(dest) & 0b11) && count) {
        *dest++ = *source;
        count--;
      }

      if(!count) return;

      // two pixels at a time
      uint32_t dpen = (*source << 16) | *source;
      uint32_t *dwd = (uint32_t *)dest;
      while(count > 1) {
        *dwd++ = dpen;
        count -= 2;
      }

      // finish off with last pixel if needed
      if(count) {
        *dest = *source;
      }
    }
  }

  // copy the source over the destination if source alpha is != 0
  // allows for faster blitting of sprites that only need 1-bit alpha
  void MASK(color_t *source, uint32_t source_step, color_t *dest, uint32_t count) {
    uint32_t s = *source & 0xff0f;
    uint8_t sa = (*source &0xf0) >> 4;

    while(count--) {
      uint8_t da = (*dest & 0xf0); // extract alpha to add back later

      // reconstruct blended colour components into original format
      if(sa != 0) {
        *dest = s | da;
      }
      dest++;

      if(source_step == 1) {
        source += source_step;

        // unpack and prepare next source pixel
        s = *source & 0xff0f;
        sa = (*source & 0xf0) >> 4;
      }
    }
  }


  // blends the source and destination based on the source alpha
  void BLEND(color_t *source, uint32_t source_step, color_t *dest, uint32_t count) {
    // unpack source into 32 bits with space for alpha multiplication
    // we start with ggggbbbbaaaarrrr and end up with
    // ------------gggg----bbbb----rrrr
    uint32_t s = (*source | ((*source & 0xf000) << 4)) & 0xf0f0f;
    uint8_t sa = (*source &0xf0) >> 4;

    while(count--) {
      if(sa == 15) {
        // full alpha, copy pixel
        *dest = *source;
      } else if(sa == 0) {
        // zero alpha, do nothing...
      } else {
        // unpack dest into 32 bits with space for alpha multiplication
        // we start with ggggbbbbaaaarrrr and end up with
        // ------------gggg----bbbb----rrrr
        uint32_t d = (*dest | ((*dest & 0xf000) << 4)) & 0xf0f0f;
        uint8_t da = (*dest & 0xf0); // extract alpha to add back later

        // blend the three channels in one go
        d = d + ((sa * (s - d) + 0x070707) >> 4);

        // reconstruct blended colour components into original format
        *dest = (d & 0x0f0f) | ((d & 0xf0000) >> 4) | da;
      }

      dest++;

      if(source_step == 1) {
        source += source_step;

        // unpack and prepare next source pixel
        s = (*source | ((*source & 0xf000) << 4)) & 0xf0f0f;
        sa = (*source & 0xf0) >> 4;
      }
    }
  }

}