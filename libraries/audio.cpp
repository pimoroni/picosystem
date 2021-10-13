#include "picosystem.hpp"

namespace picosystem {

  voice_t _chan[4];
  uint32_t _vol = 0x7f;
  uint32_t _sr = 22058;

  int16_t _wf_sin[256] = {  // sine waveform lut
    0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57,
    59, 62, 65, 67, 70, 73, 75, 78, 80, 82, 85, 87, 89, 91, 94, 96, 98, 100,
    102, 103, 105, 107, 108, 110, 112, 113, 114, 116, 117, 118, 119, 120, 121,
    122, 123, 123, 124, 125, 125, 126, 126, 126, 126, 126, 126, 126, 126, 126,
    126, 126, 125, 125, 124, 123, 123, 122, 121, 120, 119, 118, 117, 116, 114,
    113, 112, 110, 108, 107, 105, 103, 102, 100, 98, 96, 94, 91, 89, 87, 85,
    82, 80, 78, 75, 73, 70, 67, 65, 62, 59, 57, 54, 51, 48, 45, 42, 39, 36, 33,
    30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0, -3, -6, -9, -12, -15, -18, -21,
    -24, -27, -30, -33, -36, -39, -42, -45, -48, -51, -54, -57, -59, -62, -65,
    -67, -70, -73, -75, -78, -80, -82, -85, -87, -89, -91, -94, -96, -98, -100,
    -102, -103, -105, -107, -108, -110, -112, -113, -114, -116, -117, -118,
    -119, -120, -121, -122, -123, -123, -124, -125, -125, -126, -126, -126,
    -126, -126, -126, -126, -126, -126, -126, -126, -125, -125, -124, -123,
    -123, -122, -121, -120, -119, -118, -117, -116, -114, -113, -112, -110,
    -108, -107, -105, -103, -102, -100, -98, -96, -94, -91, -89, -87, -85, -82,
    -80, -78, -75, -73, -70, -67, -65, -62, -59, -57, -54, -51, -48, -45, -42,
    -39, -36, -33, -30, -27, -24, -21, -18, -15, -12, -9, -6, -3
  };

  int16_t _wf_saw[256] = {  // saw waveform lut
    -127, -126, -125, -124, -123, -122, -121, -120, -119, -118, -117, -116,
    -115, -114, -113, -112, -111, -110, -109, -108, -107, -106, -105, -104,
    -103, -102, -101, -100, -99, -98, -97, -96, -95, -94, -93, -92, -91, -90,
    -89, -88, -87, -86, -85, -84, -83, -82, -81, -80, -79, -78, -77, -76, -75,
    -74, -73, -72, -71, -70, -69, -68, -67, -66, -65, -64, -63, -62, -61, -60,
    -59, -58, -57, -56, -55, -54, -53, -52, -51, -50, -49, -48, -47, -46, -45,
    -44, -43, -42, -41, -40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30,
    -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15,
    -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
    44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
    63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
    115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128
  };

  // struct voice_t {
  //   uint32_t i; // current sample index
  //   uint32_t frequency, bend; // pitch (hz)
  //   uint32_t waveform; // shape (sine, triangle, saw, noise)
  //   uint32_t attack, decay, sustain, release; // envelope (ms)
  //   uint32_t reset, reverb; // effects (ms)
  //   uint32_t volume, noise, distort; // effects (strength 0..100)
  // };
  void play(voice_t v) {
    // convert from hz into fp8 where fractional part is waveform offset
    v.frequency   = (v.frequency  << 8) / _sr;
    v.bend        = (v.bend       << 8) / _sr;

    // convert from milliseconds into samples
    v.attack      = (v.attack     * _sr) / 1000;
    v.decay       = (v.decay      * _sr) / 1000;
    v.hold        = (v.hold       * _sr) / 1000;
    v.release     = (v.release    * _sr) / 1000;
    v.reset       = (v.reset      * _sr) / 1000;
    v.reverb      = (v.reverb     * _sr) / 1000;

    // convert from 0...100 to 0..1 fp8
    v.sustain     = (v.sustain    << 8) / 100;
    v.volume      = (v.volume     << 8) / 100;
    v.noise       = (v.noise      << 8) / 100;

    _chan[0] = v;
  }

  int8_t _get_audio_frame() {
    voice_t *v = &_chan[0];

    uint32_t duration = v->attack + v->decay + v->hold + v->release + v->reverb;

    if(v->i < duration) {
      // skip samples for distortion effect
      if(v->distort == 0 || !(v->i % v->distort)) {
        v->s = 0;

        // get waveform sample
        v->s = _wf_sin[v->t & 0xff];

        // apply envelop to waveform sample
        if(v->i < v->attack) {
          v->s *= (v->i << 8) / v->attack;
        } else if (v->i < v->attack + v->decay) {
          v->s *= (1 << 8) - (((((v->i - v->attack) << 8) / v->decay) * ((1 << 8) - v->sustain)) >> 8);
        } else if (v->i < v->attack + v->decay + v->hold) {
          v->s *= v->sustain;
        } else if (v->i < v->attack + v->decay + v->hold + v->release) {
          uint32_t r = ((v->i - v->attack - v->decay - v->hold) << 8) / v->release;
          v->s *= (((1 << 8) - r) * v->sustain) >> 8;
        } else {
          v->s = 0;
        }

        v->s >>= 8;
      }

      v->t += v->frequency;

      v->i++;

      return v->s;
    }

    return 0;
  }

}

  // uint32_t _prng_xorshift_state = 0x32B71700;

  // uint32_t _prng_xorshift_next() {
  //   uint32_t x = _prng_xorshift_state;
  //   x ^= x << 13;
  //   x ^= x >> 17;
  //   x ^= x << 5;
  //   _prng_xorshift_state = x;
  //   return x;
  // }

  // int32_t _prng_normal() {
  //   // rough approximation of a normal distribution
  //   uint32_t r0 = _prng_xorshift_next();
  //   uint32_t r1 = _prng_xorshift_next();
  //   uint32_t n = ((r0 & 0xffff) + (r1 & 0xffff) + (r0 >> 16) + (r1 >> 16)) / 2;
  //   return n - 0xffff;
  // }


  // The duration a note is played is determined by the amount of attack,
  // decay, and release, combined with the length of the note as defined by
  // the user.
  //
  // - Attack:  number of milliseconds it takes for a note to hit full volume
  // - Decay:   number of milliseconds it takes for a note to settle to sustain volume
  // - Sustain: percentage of full volume that the note sustains at (duration implied by other factors)
  // - Release: number of milliseconds it takes for a note to reduce to zero volume after it has ended
  //
  // Attack (750ms) - Decay (500ms) -------- Sustain ----- Release (250ms)
  //
  //                +         +                                  +    +
  //                |         |                                  |    |
  //                |         |                                  |    |
  //                |         |                                  |    |
  //                v         v                                  v    v
  // 0ms               1000ms              2000ms              3000ms              4000ms
  //
  // |              XXXX |                   |                   |                   |
  // |             X    X|XX                 |                   |                   |
  // |            X      |  XXX              |                   |                   |
  // |           X       |     XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX|                   |
  // |          X        |                   |                   |X                  |
  // |         X         |                   |                   |X                  |
  // |        X          |                   |                   | X                 |
  // |       X           |                   |                   | X                 |
  // |      X            |                   |                   |  X                |
  // |     X             |                   |                   |  X                |
  // |    X              |                   |                   |   X               |
  // |   X               |                   |                   |   X               |
  // |  X +    +    +    |    +    +    +    |    +    +    +    |    +    +    +    |    +
  // | X  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
  // |X   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
  // +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+--->