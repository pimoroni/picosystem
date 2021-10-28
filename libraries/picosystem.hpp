#pragma once

#include <memory>
#include <cstdint>
#include <climits>
#include <initializer_list>

#include <string>
#include <vector>

#include "pico/stdlib.h"

void init();
void update(uint32_t tick);
void draw(uint32_t tick);

namespace picosystem {

  extern uint32_t _debug;

  typedef uint16_t color_t;

  struct stat_t {
    uint32_t fps;       // current frames per second
    uint32_t idle;      // cpu idle time percentage
    uint32_t tick_us;   // last full tick time in microseconds
    uint32_t update_us; // last update() call time in microseconds
    uint32_t draw_us;   // last draw() call time in microseconds
    uint32_t flip_us;   // last flip time in microseconds
  };

  extern stat_t stats;

  struct buffer_t {
    int32_t w, h;
    color_t *data;

    color_t *p(int32_t x, int32_t y) {
      return data + (x + y * w);
    }
  };

  struct voice_t {
    int32_t  bend; // pitch bend (hz)
    uint32_t bend_ms; // bend speed (ms)
    uint32_t attack, decay, sustain, release; // envelope (ms, ms, %, ms)
    uint32_t reverb; // effects (ms)
    uint32_t noise, distort; // effects (strength 0..100)
  };

  constexpr uint32_t HFLIP  = 0x01;
  constexpr uint32_t VFLIP  = 0x02;

  using blend_func_t =
    void(*)(color_t* source, int32_t so, int32_t ss,
            color_t*   dest, uint32_t count);

  // drawing state
  extern color_t        _pen;                // pen
  extern uint8_t        _a;                  // global alpha
  extern int32_t        _cx, _cy, _cw, _ch;  // clip rect
  extern int32_t        _tx, _ty;            // text cursor position
  extern int32_t        _tlh, _tls;          // text letter height and spacing
  extern int32_t        _tlw;                // text letter width (-1: variable)
  extern uint32_t       _io, _lio;           // io state and last io state
  extern int32_t        _camx, _camy;        // camera
  extern blend_func_t   _bf;                 // blend function
  extern buffer_t      * SCREEN;             // framebuffer
  extern buffer_t      *_dt;                 // drawing target
  extern buffer_t      * SPRITESHEET;        // inbuilt spritesheet
  extern buffer_t      *_ss;                 // sprite sheet
  extern uint8_t       *_font;               // font data

  // audio state
  extern voice_t        _v;                   // current voice
  extern uint32_t       _bpm;                 // beats per minute

  // __ers
  extern float          _fsin_lut[256];       // fast sin lut
  constexpr float       _PI = 3.1415927f;

  // state
  void        pen();
  void        pen(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 15);
  void        pen(color_t p);
  void        alpha();
  void        alpha(uint8_t a);
  void        clip();
  void        clip(int32_t x, int32_t y, uint32_t w, uint32_t h);
  void        blend();
  void        blend(blend_func_t bf);
  void        target();
  void        target(buffer_t *dt);
  void        camera();
  void        camera(int32_t x, int32_t y);
  void        spritesheet();
  void        spritesheet(buffer_t *ss);
  void        cursor();
  void        cursor(int32_t x, int32_t y);
  void        font(
                int32_t w = -1, int32_t h = 8, int32_t s = 1,
                uint8_t *data = nullptr);

  // primitives
  void        clear();
  void        pixel(int32_t x, int32_t y);
  void        hline(int32_t x, int32_t y, int32_t c);
  void        vline(int32_t x, int32_t y, int32_t c);
  void        poly(const int32_t *p, uint32_t l);
  void        poly(const std::initializer_list<int32_t> &pts);
  void        fpoly(const int32_t *p, uint32_t l);
  void        fpoly(const std::initializer_list<int32_t> &pts);
  void        rect(int32_t x, int32_t y, int32_t w, int32_t h);
  void        frect(int32_t x, int32_t y, int32_t w, int32_t h);
  void        circle(int32_t x, int32_t y, int32_t r);
  void        fcircle(int32_t x, int32_t y, int32_t r);
  void        ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry);
  void        fellipse(int32_t x, int32_t y, int32_t rx, int32_t ry);
  void        line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
  void        blit(
                buffer_t *source,
                int32_t x, int32_t y, int32_t w, int32_t h,
                int32_t dx, int32_t dy,
                uint32_t flags = 0);
  void        blit(
                buffer_t *source,
                int32_t sx, int32_t sy, int32_t sw, int32_t sh,
                int32_t dx, int32_t dy, int32_t dw, int32_t dh,
                uint32_t flags = 0);
  void        sprite(uint32_t i, int32_t x, int32_t y);
  void        sprite(uint32_t i, int32_t x, int32_t y, int32_t cx, int32_t cy);
  void        sprite(
                uint32_t i,
                int32_t x, int32_t y, int32_t cx, int32_t cy,
                int32_t dw, int32_t dh);
  void        text(const char &c, int32_t x, int32_t y);
  void        text(const char &c);
  void        text(
                const std::string &t,
                int32_t x, int32_t y,
                int32_t wrap = -1);
  void        text(
                const std::string &t,
                int32_t wrap = -1);

  // blend functions
  void        COPY(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        ALPHA(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        MASK(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        PEN(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        DARKEN(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        LIGHTEN(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        ADD(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        SUBTRACT(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        MULTIPLY(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);
  void        DISSOLVE(
                color_t* ps, int32_t so, int32_t ss, color_t* pd, uint32_t c);

  // audio
  void        play(
                voice_t voice, uint32_t frequency,
                uint32_t duration = 500, uint32_t volume = 100);
  uint8_t     audio_sample(uint32_t ms);
  uint32_t    audio_position();
  bool        audio_playing();

  // utility
  std::string str(float v, uint8_t precision = 2);
  std::string str(int32_t v);
  std::string str(std::size_t v);
  std::string str(uint32_t v);
  color_t     rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 15);
  color_t     hsv(float h, float s, float v, float a = 1.0f);
  buffer_t*   buffer(uint32_t w, uint32_t h, void *data = nullptr);
  voice_t     voice(
                uint32_t attack = 100, uint32_t decay = 50,
                uint32_t sustain = 80, uint32_t release = 100,
                int32_t bend = 0, uint32_t bend_ms = 0, uint32_t reverb = 0,
                uint32_t noise = 0, uint32_t distort = 0);
  uint32_t    time();
  uint32_t    time_us();
  void        sleep(uint32_t d);
  bool        intersects(
                int32_t  x, int32_t  y, int32_t  w, int32_t  h,
                int32_t cx, int32_t cy, int32_t cw, int32_t ch);
  void        intersection(
                int32_t &x, int32_t &y, int32_t &w, int32_t &h,
                int32_t cx, int32_t cy, int32_t cw, int32_t ch);
  bool        contains(
                int32_t  x, int32_t  y,
                int32_t cx, int32_t cy, int32_t cw, int32_t ch);
  bool        contains(
                int32_t  x, int32_t  y, int32_t  w, int32_t  h,
                int32_t cx, int32_t cy, int32_t cw, int32_t ch);
  void        measure(
                const std::string &t,
                int32_t &w, int32_t &h,
                int32_t wrap = -1);
  std::vector<std::string> split(const std::string& t, char d = '\n');
  float       fsin(float v);
  float       fcos(float v);
  color_t     mix(color_t c1, color_t c2, uint32_t m);

  // hardware
  bool        pressed(uint32_t b);
  bool        button(uint32_t b);
  uint32_t    battery();
  void        led(uint8_t r, uint8_t g, uint8_t b);
  void        backlight(uint8_t b);

  // weird and wonderful...
  //void        screenshot();

  // internal methods - do not call directly, will change!
  void       _logo();
  void       _init_hardware();
  void       _start_audio();
  uint32_t   _gpio_get();
  float      _battery_voltage();
  void       _reset_to_dfu();
  void       _wait_vsync();
  void       _flip();
  bool       _is_flipping();
  void       _camera_offset(int32_t &x, int32_t &y);
  void       _camera_offset(bool enable);
  void       _update_audio();
  void       _play_note(uint32_t f, uint32_t v);

  // input pins
  enum button {
    UP    = 23,
    DOWN  = 20,
    LEFT  = 22,
    RIGHT = 21,
    A     = 18,
    B     = 19,
    X     = 17,
    Y     = 16
  };

  // assets
  const extern uint8_t _picosystem_logo[550];
  const extern uint8_t _default_font[96][9];
  const extern color_t _default_sprite_sheet[16384];

  #ifndef NO_SPRITESHEET
  enum _sprites_ids {
    // 00+
    CHERRY, APPLE, BANANA, ORANGE, AUBERGINE, CARROT, POTATO, LEAF,
    // 08+
    PARSNIP, MEAT, BEAN, RADISH, STRAWBERRY, PUMPKIN, CUCUMBER, SALT1,
    // 10+
    SWORD1, SWORD2, SWORD3, AXE1, DAGGER, MACE1, BOW, ARROW1,
    // 18+
    ARROW2, ARROW3, TRIDENT1, SPEAR, TRIDENT2, MACHINEGUN, PISTOL, SHOTGUN,
    // 20+
    GEM1, GEM2, GEM3, GEM4, GEM5, GEM6, AMULET, FOLDER,
    // 28+
    COIN1, SKULL, TICK, CROSS, IDOL1, IDOL2, IDOL3, IDOL4,
    // 30+
    SWEETCORN, BROCCOLI, BISCUIT, BERRIES, SCEPTRE, FIRE, LASERGUN1, LASERGUN2,
    // 38+
    HELMET1, HELMET2, HELMET3, HELMET4, BOOT1, BOOT2, BOOT3, BOOT4,
    // 40+
    HEART1, SLIME, COIN2, SWORD4, SWORD5, __SPR_45, DYNAMITE, MEDPACK,
    // 48+
    GOGGLES, HELMET5, BOMB, OILCAN, SHIRT1, SHIRT2, SPEECH, DREAM,
    // 50+
    HEART2, HEART3, HEART4, __SPR_53, TORCH, ICE, CARD, DIE,
    // 58+
    HELMET6, HELMET7, UPARROW, RING1, RING2, RING3, RING4, POUCH,
    // 60+
    TOKEN, VIAL1, VIAL2, RING5, RING6, BLOCK1, BLOCK2, BLOCK3,
    // 68+
    BLOCK4, BLOCK5, BLOCK6, BLOCK7, BLOCK8, BLOCK9, SHIELD1, SACK,
    // 70+
    BUTTON, VIAL3, VIAL4, TOOTH, BONE, HALFBLOCK1, HALFBLOCK2, HALFBLOCK3,
    // 78+
    HALFBLOCK4, HALFBLOCK5, HALFBLOCK6, HALFBLOCK7, HALFBLOCK8, HALFBLOCK9, SHIELD2, SHIELD3,
    // 80+
    FLOWER1, FLOWER2, FLOWER3, FLOWER4, STAFF1, STAFF2, STAFF3, STICK1,
    // 88+
    SPIKEBALL, MACE2, AXE2, __SPR_8B, SWORD6, SWORD7, SWORD8, CROWN1,
    // 90+
    GOLD, GEM7, GEM8, GEM9, AXE3, MUSHROOM, SALT2, STICK2,
    // 98+
    VIAL5, VIAL6, DOT, HELMET8, SWORD9, SWORD10, SWORD11, CROWN2,
    // A0+
    TOMATO, LEAF2, LEAF3, LEAF4, LEAF5, MUSHROOM2, STUMP, STAR,
    // A8+
    BIRD, BAUBLE, KEY1, KEY2, KEY3, __SPR_AC, NECKLACE1, NECKLACE2, NECKLACE3,
    // B0+
    CATERPILLAR, LEAF2E, LEAF3E, LEAF4E, LEAF5E, FLOWER5, FLOWER6, FLOWERS,
    // B8+
    DOLL, EYE, FISH, BURGER, LIGHT, NOTE, DROP, DIPPER,
    // C0+
    AVOCADO, VIAL7, VIAL8, VIAL9, VIAL10, CANDYCANE, EGG, SCYTHE1,
    // C8+
    SCYTHE2, SCYTHE3, CHEESE1, CHEESE2, CURSOR1, __SPR_CD, FIREBALL1, FIREBALL2,
    // D0+
    MEDIC, RING7, FLOWER7, FLOWER8, PAN, CONSOLE, HEADPHONES, BRICK1,
    // D8+
    BRICK2, BRICK3, SHRIMP, __SPR_DB, CURSOR2, BLUEDOT, __SPR_DE, __SPR_DF,
    // E0+
    BOOK, __SPR_E1, PHONE, ROSE, BLOCK10, BLOCK11, ICEBLOCK, ROCKET1,
    // E8+
    ROCKET2, BIRD2, GOBLIN, __SPR_EB, CARCAS, RETICLE, __SPR_EE, __SPR_EF,
    // F0+
    CLOCK, ALERT, __SPR_F2, QUESTION, BLOCK12, BLOCK13, HARP, CHECKER1,
    // F8+
    CHECKER2, FLAG1, FLAG2, FLAG3, FLAG4, STOP, MISSILE, BEAMSPLASH
    // can we better name the __SPR_XX sprites?
  };
  #endif

  enum note {
    _ = 0,
    C0 = 16, Cs0 = 17, Db0 = 17, D0 = 18, Ds0 = 19, Eb0 = 19,
    E0 = 20, F0 = 21, Fs0 = 23, Gb0 = 23, G0 = 24, Gs0 = 25,
    Ab0 = 25, A0 = 27, As0 = 29, Bb0 = 29, B0 = 30, C1 = 32,
    Cs1 = 34, Db1 = 34, D1 = 36, Ds1 = 38, Eb1 = 38, E1 = 41,
    F1 = 43, Fs1 = 46, Gb1 = 46, G1 = 49, Gs1 = 51, Ab1 = 51,
    A1 = 55, As1 = 58, Bb1 = 58, B1 = 61, C2 = 65, Cs2 = 69,
    Db2 = 69, D2 = 73, Ds2 = 77, Eb2 = 77, E2 = 82, F2 = 87,
    Fs2 = 92, Gb2 = 92, G2 = 98, Gs2 = 103, Ab2 = 103, A2 = 110,
    As2 = 116, Bb2 = 116, B2 = 123, C3 = 130, Cs3 = 138, Db3 = 138,
    D3 = 146, Ds3 = 155, Eb3 = 155, E3 = 164, F3 = 174, Fs3 = 185,
    Gb3 = 185, G3 = 196, Gs3 = 207, Ab3 = 207, A3 = 220, As3 = 233,
    Bb3 = 233, B3 = 246, C4 = 261, Cs4 = 277, Db4 = 277, D4 = 293,
    Ds4 = 311, Eb4 = 311, E4 = 329, F4 = 349, Fs4 = 369, Gb4 = 369,
    G4 = 392, Gs4 = 415, Ab4 = 415, A4 = 440, As4 = 466, Bb4 = 466,
    B4 = 493, C5 = 523, Cs5 = 554, Db5 = 554, D5 = 587, Ds5 = 622,
    Eb5 = 622, E5 = 659, F5 = 698, Fs5 = 739, Gb5 = 739, G5 = 783,
    Gs5 = 830, Ab5 = 830, A5 = 880, As5 = 932, Bb5 = 932, B5 = 987,
    C6 = 1046, Cs6 = 1108, Db6 = 1108, D6 = 1174, Ds6 = 1244, Eb6 = 1244,
    E6 = 1318, F6 = 1396, Fs6 = 1479, Gb6 = 1479, G6 = 1567, Gs6 = 1661,
    Ab6 = 1661, A6 = 1760, As6 = 1864, Bb6 = 1864, B6 = 1975, C7 = 2093,
    Cs7 = 2217, Db7 = 2217, D7 = 2349, Ds7 = 2489, Eb7 = 2489, E7 = 2637,
    F7 = 2793, Fs7 = 2959, Gb7 = 2959, G7 = 3135, Gs7 = 3322, Ab7 = 3322,
    A7 = 3520, As7 = 3729, Bb7 = 3729, B7 = 3951, C8 = 4186, Cs8 = 4434,
    Db8 = 4434, D8 = 4698, Ds8 = 4978, Eb8 = 4978, E8 = 5274, F8 = 5587,
    Fs8 = 5919, Gb8 = 5919, G8 = 6271, Gs8 = 6644, Ab8 = 6644, A8 = 7040,
    As8 = 7458, Bb8 = 7458, B8 = 7902,
  };

}
