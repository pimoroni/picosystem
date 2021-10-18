#include "picosystem.hpp"

namespace picosystem {

  voice_t  _voice;
  uint32_t _ms;
  uint32_t _frequency;
  uint32_t _duration;
  uint32_t _volume = 100;

  void play(voice_t voice,
    uint32_t frequency, uint32_t duration, uint32_t volume) {
    _frequency = frequency;
    _duration = duration;
    _volume = volume;
    _voice = voice;
    _ms = 0;
  }

  uint32_t audio_position() {
    return _ms;
  }

  uint8_t audio_sample(uint32_t ms) {
    // calculate full duration including release and reverb
    uint32_t full_duration = _duration + _voice.release + _voice.reverb;
    uint32_t sample = 0;
    if(ms < full_duration) {
      // skip samples for distortion effect
      if(_voice.distort) {
        ms -= (ms % _voice.distort);
      }

      // grab volume as start point for sample
      sample = _volume;

      // apply envelop to waveform sample
      uint32_t env = 0;
      if(ms < std::min(_duration, _voice.attack)) {
        env = (ms << 8) / _voice.attack;
      } else if (ms < std::min(_duration, _voice.attack + _voice.decay)) {
        env = (1 << 8) - (((((ms - _voice.attack) << 8) / _voice.decay) * ((1 << 8) - ((_voice.sustain << 8) / 100))) >> 8);
      } else if (ms < _duration) {
        env = (_voice.sustain << 8) / 100;
      } else if (ms < _duration + _voice.release) {
        uint32_t r = ((ms - _duration) << 8) / _voice.release;
        env = (((1 << 8) - r) * ((_voice.sustain << 8) / 100)) >> 8;
      }
      sample *= env;
      sample >>= 8;

      // apply noise
      if(_voice.noise) {
        int32_t n = (std::rand() % _voice.noise) - (_voice.noise / 2);
        sample = int32_t(sample) + n < 0 ? 0 : (int32_t(sample) + n > 100 ? 100 : sample + n);
      }

      if(_voice.reverb && ms > _voice.reverb) {
        uint8_t rs = audio_sample(ms - _voice.reverb);
        sample = ((sample * ms) / full_duration) + ((rs * (full_duration - ms)) / full_duration);
      }
    }

    return sample;
  }

  void _update_audio() {
    // pitch bend
    if(_voice.bend && _voice.bend_ms) {
      _frequency += _voice.bend * (_ms / _voice.bend_ms);
    }
    uint32_t sample = audio_sample(_ms++);
    _play_note(_frequency, sample);
  }
}
