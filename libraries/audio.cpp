#include "picosystem.hpp"

namespace picosystem {

  voice_t _v;
  uint8_t _last_audio_sample;

  void play(voice_t v) {
    _v = v;
  }

  uint8_t last_audio_sample() {
    return _last_audio_sample;
  }

  uint32_t _get_audio_at_ms(int32_t ms) {
    uint32_t duration = _v.attack + _v.decay + _v.hold + _v.release + _v.reverb;
    uint32_t sample = 0;
    if(ms > 0 && ms < duration) {
      if(ms % 10 == 0) {
        _v.frequency += _v.bend;
      }

      // skip samples for distortion effect
      if(_v.distort == 0 || !(ms % _v.distort)) {
        // grab volume as start point for sample, convert to fp8
        sample = _v.volume;

        // apply envelop to waveform sample
        uint32_t env = 0;
        if(ms < _v.attack) {
          env = (ms << 8) / _v.attack;
        } else if (ms < _v.attack + _v.decay) {
          env = (1 << 8) - (((((ms - _v.attack) << 8) / _v.decay) * ((1 << 8) - ((_v.sustain << 8) / 100))) >> 8);
        } else if (ms < _v.attack + _v.decay + _v.hold) {
          env = (_v.sustain << 8) / 100;
        } else if (ms < _v.attack + _v.decay + _v.hold + _v.release) {
          uint32_t r = ((ms - _v.attack - _v.decay - _v.hold) << 8) / _v.release;
          env = (((1 << 8) - r) * ((_v.sustain << 8) / 100)) >> 8;
        }
        sample *= env;

        // apply noise
        if(_v.noise) {
          int32_t n = ((std::rand() % _v.noise) - (_v.noise >> 1)) << 8;
          sample += n;
          sample = sample > (100 << 8) ? (100 << 8) : sample;
        }

        sample >>= 8;

        if(ms > _v.reverb) {
          uint32_t rs = _get_audio_at_ms(ms - _v.reverb);
          sample = ((sample * ms) / duration) + ((rs * (duration - ms)) / duration);
        }

      }
    }

    return sample;
  }

  void _update_audio() {
    uint32_t sample = _get_audio_at_ms(_v.ms++);
    _last_audio_sample = sample;
    _play_note(_v.frequency, sample);
  }
}
