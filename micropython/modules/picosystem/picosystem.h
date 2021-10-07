// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// input pins
enum button_pin {
  UP    = 23,
  DOWN  = 20,
  LEFT  = 22,
  RIGHT = 21,
  A     = 18,
  B     = 19,
  X     = 17,
  Y     = 16
};

enum blend_mode {
  MODE_COPY = 0,
  MODE_BLEND,
  MODE_MASK
};

// Declare the functions we'll make available in Python
//extern mp_obj_t picosystem_init();
//extern mp_obj_t picosystem_update(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
//extern mp_obj_t picosystem_draw();

extern mp_obj_t picosystem_init();
extern mp_obj_t picosystem_tick();

// state
extern mp_obj_t picosystem_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_blend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_target(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_camera(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_spritesheet(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

// primitives
extern mp_obj_t picosystem_clear();
extern mp_obj_t picosystem_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_hline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_vline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_rect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_poly(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_frect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_fcircle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_fpoly(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_blit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_sprite(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

//void        COPY(
//              color_t* source, uint32_t source_step,
//              color_t* dest, uint32_t count);
//void        BLEND(
//              color_t* source, uint32_t source_step,
//              color_t* dest, uint32_t count);
//void        MASK(
//              color_t* source, uint32_t source_step,
//              color_t* dest, uint32_t count);

// utility
extern mp_obj_t picosystem_str(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

//uint32_t    time();
//uint32_t    time_us();
//void        sleep(uint32_t d);
//void        sleep_us(uint32_t d);

extern mp_obj_t picosystem_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_intersection(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_contains(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t picosystem_wrap(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

//float       fsin(float v);
//float       fcos(float v);

// hardware
extern mp_obj_t picosystem_pressed(mp_obj_t b_obj);
extern mp_obj_t picosystem_button(mp_obj_t b_obj);
extern mp_obj_t picosystem_battery();
extern mp_obj_t picosystem_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picosystem_backlight(mp_obj_t b_obj);
