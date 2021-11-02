
#include "picosystem.hpp"

namespace picosystem {

  uint32_t _char_width(char c) {
    return _tlw == -1 ? _font[(c - 32) * 9] : _tlw;
  }

  // draws a character at the current cursor position
  void text(const char &c) {
    const uint8_t *p = &_font[(c - 32) * 9];

    // get character width
    uint8_t w = *p++;

    // centre glyph if in fixed with mode
    uint8_t xo = 0;
    if(_tlw != -1) {
      xo = (_tlw - w) >> 1;
    }

    // draw character
    for(int32_t y = _ty; y < _ty + 8; y++) {
      uint8_t pr = *p++;
      for(int32_t x = _tx + xo; x < _tx + xo + w; x++) {
        if(pr & 0x80) pixel(x, y);
        pr <<= 1;
      }
    }

    _tx += _char_width(c) + _tls;
  }

  bool _matches(const std::string &t, const std::string &m, std::size_t &i) {
    std::size_t j = 0;
    while(j < m.size()) {
      if(t[i + j] != m[j]) {
        return false;
      }
      j++;
    }

    // consume characters if a match
    i += m.size();
    return true;
  }

  uint32_t _skip_escape_code(const std::string &t, std::size_t &i) {
    uint32_t l = 0;
    i++;
    if(_matches(t, "pen", i)) {
      i += 3;
    }
    if(_matches(t, "spr", i)) {
      i += 2;
      l = 8;
    }
    return l;
  }

  // search for end of the next word (including any e)
  uint32_t _next_word_length(const std::string &t, std::size_t i) {
    // search through until we find a non word character to break on
    uint32_t l = 0;
    while(true) {
      char c = t[i];
      if(c <= 32) {
        break;
      }

      if(c == '\\') {
        l += _skip_escape_code(t, i);
      } else {
        l += _char_width(c) + _tls;
        i++;
      }
    }
    return l;
  }

  uint8_t _hex_to_int(char c) {
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'A' && c <= 'F') return (c - 'A') + 10;
    if(c >= 'a' && c <= 'f') return (c - 'a') + 10;
    return 0;
  }

  void _parse_escape_code(const std::string &t, std::size_t &i) {
    i++;
    if (_matches(t, "pen", i) ){
      uint8_t r = _hex_to_int(t[i++]);
      uint8_t g = _hex_to_int(t[i++]);
      uint8_t b = _hex_to_int(t[i++]);
      uint8_t a = _hex_to_int(t[i]);
      pen(r, g, b, a);
    }

    if (_matches(t, "spr", i) ){
      uint8_t si = std::stoi(t.substr(i, 3));
      i += 2;
      sprite(si, _tx, _ty);
      _tx += 8;
    }
  }

  void measure(const std::string &t, int32_t &w, int32_t &h, int32_t wrap) {
    w = 0; h = 0;

    // save cursor position for wrapping new lines
    int32_t tx = 0, ty = 0;
    bool in_word = false;

    for(std::size_t i = 0; i < t.size(); i++) {
      char c = t[i];

      switch(c) {
        case '\n': {
          tx  = 0;
          ty += _tlh;
        }break;

        case ' ': {
          tx += _font[0];
        }break;

        case '\t': {
          tx += _font[0] * 10; // four spaces
        }break;

        case '\\': {
          // special code - read ahead and do something...
          tx += _skip_escape_code(t, i);
        }break;
      }

      if(c > 32 && c != '\\') {
        // check length to end of word
        if(!in_word) {
          uint32_t nwl = _next_word_length(t, i);
          if(wrap != -1 && tx + nwl > wrap) {
            tx  = 0;
            ty += _tlh;
          }
        }

        tx += _char_width(c) + _tls;
        w = std::max(w, tx);
      }

      in_word = c > 32 && c != '\\';
    }

    // set cursor to start of next line
    h = ty + _tlh;
  }

  void text(const std::string &t, int32_t wrap) {
    // save cursor position for wrapping new lines
    int32_t _stx = _tx;
    int32_t _wtx = _tx + wrap;
    bool in_word = false;

    for(std::size_t i = 0; i < t.size(); i++) {
      char c = t[i];

      switch(c) {
        case '\n': {
          _tx  = _stx;
          _ty += _tlh;
        }break;

        case ' ': {
          _tx += _font[0];
        }break;

        case '\t': {
          _tx += _font[0] * 10;
        }break;

        case '\\': {
          // special code - read ahead and do something...
          _parse_escape_code(t, i);
        }break;
      }

      if(c > 32 && c != '\\') {
        // check length to end of word
        if(!in_word) {
          int32_t nwl = _next_word_length(t, i);
          if(wrap != -1 && _tx + nwl > _wtx) {
            _tx  = _stx;
            _ty += _tlh;
          }
        }

        text(c);
      }

      in_word = c > 32 && c != '\\';
    }

    // set cursor to start of next line
    _ty += _tlh;
    _tx = _stx;
  }

  void text(const std::string &t, int32_t x, int32_t y, int32_t wrap) {
    _camera_offset(x, y);
    _tx = x; _ty = y;
    text(t, wrap);
  }

}