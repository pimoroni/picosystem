#include <string.h>

#include "picosystem.hpp"

namespace picosystem {

  // copy the source over the destination, ignoring alpha channel
  void COPY(color_t *ps, uint32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      *pd++ = *(ps + (so >> 16));
      so += ss;
    }
  }

  // copy the source over the destination if source alpha is != 0
  // allows for faster blitting of sprites that only need 1-bit alpha
  void MASK(color_t *ps, uint32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      uint32_t s = *(ps + (so >> 16));
      if(s & 0x00f0) { // if has alpha component
        *pd = s;
      }
      pd++;
      so += ss;
    }
  }

  // blends the source and destination based on the source alpha
  void BLEND(color_t *ps, uint32_t so, int32_t ss, color_t *pd, uint32_t c) {
    while(c--) {
      // unpack source into 32 bits with space for alpha multiplication
      // we start with ggggbbbbaaaarrrr and end up with
      // ------------gggg----bbbb----rrrr
      uint32_t ts = *(ps + (so >> 16));
      uint32_t s = (ts | ((ts & 0xf000) << 4)) & 0xf0f0f;
      uint8_t sa = (ts & 0x00f0) >> 4;

      // unpack dest into 32 bits with space for alpha multiplication
      // we start with ggggbbbbaaaarrrr and end up with
      // ------------gggg----bbbb----rrrr
      uint32_t d = (*pd | ((*pd & 0xf000) << 4)) & 0xf0f0f;
      uint8_t da = (*pd & 0xf0); // extract alpha to add back later

      // blend the three channels in one go
      d = d + ((sa * (s - d) + 0x070707) >> 4);

      // reconstruct blended colour components into original format
      *pd = (d & 0x0f0f) | ((d & 0xf0000) >> 4) | da;

      pd++;
      so += ss;
    }
  }

}