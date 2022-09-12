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
    bool alloc;

    color_t *p(int32_t x, int32_t y) {
      return data + (x + y * w);
    }

    ~buffer_t() {
      if (alloc) delete data;
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
  void        hline(int32_t x, int32_t y, int32_t l);
  void        vline(int32_t x, int32_t y, int32_t l);
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
  void        sprite(
                uint32_t i,
                int32_t x, int32_t y, int32_t cx, int32_t cy,
                int32_t dw, int32_t dh, uint32_t flags);
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
  voice_t     voice(
                uint32_t attack = 100, uint32_t decay = 50,
                uint32_t sustain = 80, uint32_t release = 100,
                int32_t bend = 0, uint32_t bend_ms = 0, uint32_t reverb = 0,
                uint32_t noise = 0, uint32_t distort = 0);
  void        play(
                voice_t voice, uint32_t frequency,
                uint32_t duration = 500, uint32_t volume = 100);
  uint8_t     audio_sample(uint32_t ms);
  int32_t     position();
  bool        playing();

  // utility
  std::string str(float v, uint8_t precision = 2);
  std::string str(int32_t v);
  std::string str(std::size_t v);
  std::string str(uint32_t v);
  color_t     rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 15);
  color_t     hsv(float h, float s, float v, float a = 1.0f);
  buffer_t*   buffer(uint32_t w, uint32_t h, void *data = nullptr);
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
  color_t     mix(color_t c1, color_t c2, uint8_t m);

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
    CHERRY, TOMATO, BANANA, ORANGE, EGGPLANT, CARROT, POTATO, SPINACH,
    // 08+
    GINGER, MEAT, BEANS, TURNIP, STRAWBERRY, PUMPKIN, CUCUMBER, RICE_BOWL,
    // 10+
    CLAW, SWORD_WOOD, SWORD_FROST, AXE, SAIS, MORNING_STAR, BOW_HUNTERS, ARROW,
    // 18+
    ARROW_FIRE, ARROW_ICE, TRIDENT, SPEAR, TRIDENT_FIRE, MACHINEGUN, PISTOL, SHOTGUN,
    // 20+
    ORB_GREEN, ORB_RED, ORB_YELLOW, ORB_BLUE, CANDY, RUBY_BLUE, BUTTON, OFFICIAL_LETTER,
    // 28+
    COIN_A, SKULL, UI_CHECK, UI_CANCEL, ARMOR_WOOD, ARMOR_IRON, ARMOR_GOLD, ARMOR_DIAMOND,
    // 30+
    CORN, BROCCOLI, COOKIE, RADISHES, MALLET_FROST, CAMPFIRE, SHOTGUN_AUTO, PISTOL_AUTO,
    // 38+
    BODY_PROTECTOR, HAT_WIZARD, HELMET_IRON, HELMET_VIKING, BOOTS_LEATHER, BOOTS_IRON, BOOTS_GOLD, BOOTS_DIAMOND,
    // 40+
    UI_HEART, SLIME_GREEN, COIN_B, SWORD_IRON, DAGGER_IRON, VEST_LEATHER, DYNAMITE, HEALTH_KIT,
    // 48+
    PILOT_GOGGLES, HAT_DEMON_KING, BOMB, WATERING_CAN, MASK_DEMON, MASK_ELVEN, SPEECH_BUBBLE_A, SPEECH_BUBBLE_B,
    // 50+
    UI_HEART_HALF, UI_HEART_EMPTY, UI_HEART_FROZEN, BUTTERFLY, TORCH, STONE_PIECE, PLAYING_CARD, DIE,
    // 58+
    HAT_HIGH_KNIGHT_DECORATED, HAT_HIGH_KNIGHT, UI_ARROW_UP, RING_GOLDEN, RING_DEMONS, RING_FROST, RING_NATURE, POUCH_LITTLE,
    // 60+
    COIN_SILVER, POTION_HEALTH, POTION_MAGIC, RING_DIAMOND, RING_EMERALD, BLOCK_STACK_WOOD, BLOCK_STACK_STONE, BLOCK_STACK_SAND,
    // 68+
    BLOCK_STACK_BIOLOGIC, BLOCK_STACK_LAVA, BLOCK_STACK_IRON, BLOCK_STACK_ICE, BLOCK_STACK_DARK, BLOCK_STACK_GOLD, SHIELD_KNIGHT, POUCH_LARGE,
    // 70+
    COIN_GOLDEN, POTION_HEALTH_EMPTY, POTION_MAGIC_EMPTY, WINGS, BONE, BLOCK_WOOD, BLOCK_STONE, BLOCK_SAND,
    // 78+
    BLOCK_BIOLOGIC, BLOCK_LAVA, BLOCK_IRON, BLOCK_ICE, BLOCK_DARK, BLOCK_GOLD, SHIELD_MIRROR, SHIELD,
    // 80+
    FLOWER_DANDELION, FLOWER_TULIP_RED, GRASS, FLOWER_TULIP_BLUE, WAND_FIRE, SPEAR_FROST, MACHETE, LOG,
    // 88+
    MINE, MORNING_STAR_LARGE, AXE_DOUBLE, STONE_SMALL, SWORD_SILVER, SWORD_SILVER_GREAT, SWORD_DIAMOND, CROWN,
    // 90+
    GOLD, DIAMOND, RUBY, EMERALD, AXE_DOUBLE_GOLD, POISON_MUSHROOM, ROPE, LOG_WITH_LEAF,
    // 98+
    POTION_SPEED_EMPTY, POTION_SPEED, EMERALD_CRYSTAL, HAT_SEAMONSTER, DAGGER_SILVER, DAGGER_SILVER_RARE, DAGGER_DIAMOND, CROWN_KINGS,
    // A0+
    APPLE, LEAF_SMALL, LEAF_ACORN, LEAF_LARGE, LEAF_LONG, MUSHROOM, WOOD, UI_STAR,
    // A8+
    BIRD, GREEN_BALL, KEY_IRON, KEY_GOLD, KEY_GOLD_WORM, AMULET_FROST, AMULET_FIRE, AMULET_FOREST,
    // B0+
    WORM, LEAF_SMALL_EATEN, LEAF_ACORN_EATEN, LEAF_LARGE_EATEN, LEAF_LONG_EATEN, FLOWER_SMALL, FLOWER_MEDIUM, FLOWER_LARGE,
    // B8+
    TOTEM_IRON_SMALL, EYEBALL, FISH, BURGER, UI_SUN, NOTE, WATER, LADLE,
    // C0+
    AVOCADO, POTION_WHITE, POTION_YELLOW, POTION_GREEN, POTION_RED, CANDYCANE, EGG_FRIED, SICKLE_IRON,
    // C8+
    GRAPPLING_HOOK, CLIMBING_HOOK, CHEESE_A, CHEESE_B, UI_SELECT, UI_SETTING_SELECT, SPELL_FIRE, SPELL_FIRE_MISSILE,
    // D0+
    MEDICAL_KIT, RING_DEATH, BLUEBERRIES, FLOWER_PINK, PAN, COMPUTER, HEADPHONES, STONE_TABLET_EMPTY,
    // D8+
    STONE_TABLET_WRITTEN_HALF, STONE_TABLET_WRITTEN, DEMON_RED, BURNING_SPEAR, UI_CURSOR, SPELL_MANA, SPELL_MANA_RAIN, SPELL_MANA_MISSILE,
    // E0+
    BOOK_LARGE, CABBAGE, CONTAINER, FLOWER_ROSE, UI_SPACER_A, UI_SPACER_B, GLASS, UI_DECREASE,
    // E8+
    UI_INCREASE, EAGLE, DEMON_GREEN, DEMON_BLUE, CORPSE, UI_CURRENT, SPELL_EMERALD_SPLASH, SPELL_EMERALD_THUNDER,
    // F0+
    UI_CLOCK, UI_ATTENTION, BATTERY_CELL, UI_QUESTION_MARK, UI_DISABLED_A, UI_DISABLED_B, BOW, START_MARKER,
    // F8+
    SMALL_START_MARKER, FLAG_A, FLAG_B, FLAG_C, FLAG_D, UI_ERROR, SPELL_EXPLOSION, SPELL_EXPLOSIVE_MISSILE
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
