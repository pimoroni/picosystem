#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "libraries/picosystem.hpp"

using namespace picosystem;

extern "C" {
#include "picosystem.h"
#include "math.h"

//#define NOT_INITIALISED_MSG     "Cannot call this function, as picodisplay is not initialised. Call picodisplay.init(<bytearray>) first."

//mp_obj_t picodisplay_buf_obj;

// mp_obj_t picodisplay_init(mp_obj_t buf_obj) {
//     mp_buffer_info_t bufinfo;
//     mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_RW);
//     picodisplay_buf_obj = buf_obj;
//     if(display == nullptr)
//         display = new PicoDisplay((uint16_t *)bufinfo.buf);
//     display->init();
//     return mp_const_none;
// }

uint32_t update_rate_ms = 10;
uint32_t pending_update_ms = 0;
uint32_t last_ms = time();

uint32_t tick = 0;


mp_obj_t picosystem_init() {

  _init_hardware();

  // setup lut for fast sin/cos functions
  for(uint32_t i = 0; i < 256; i++) {
    _fsin_lut[i] = sin((_PI * 2.0f) * (float(i) / 256.0f));
  }

  #ifndef NO_STARTUP_LOGO
    // fade in logo by ramping up backlight
    pen(0, 0, 0); clear();
    pen(15, 15, 15); _logo();
    for(int i = 0; i < 75; i++) {
      backlight(i);
      _wait_vsync();
      _flip();
    }

    sleep(300); // ...and breathe out...

    // fade out logo in 16 colour steps
    for(int i = 15; i >= 0; i--) {
      pen(0, 0, 0); clear();
      pen(i, i, i); _logo();
      _wait_vsync();
      _flip();

      sleep(20);
    }
  #else
    backlight(75);
  #endif

  sleep(300);

  pen(0, 0, 0); clear();

  // call users init() function so they can perform any needed
  // setup for world state etc
  //init();

  update_rate_ms = 10;
  pending_update_ms = 0;
  last_ms = time();

  tick = 0;

  _io = _gpio_get();

  return mp_const_none;
}

mp_obj_t picosystem_tick() {
  //while(true) {
    uint32_t ms = time();

    // work out how many milliseconds of updates we're waiting
    // to process and then call the users update() function as
    // many times as needed to catch up
    pending_update_ms += (ms - last_ms);
    while(pending_update_ms >= update_rate_ms) {
      _lio = _io;
      _io = _gpio_get();

      //update(tick++);
      pending_update_ms -= update_rate_ms;
    }

    // if current flipping the framebuffer in the background
    // then wait until that is complete before allow the user
    // to render
    while(_is_flipping()) {}

    // call user render function to draw world
    //draw();

    // wait for the screen to vsync before triggering flip
    // to ensure no tearing
    _wait_vsync();

    // flip the framebuffer to the screen
    _flip();

    last_ms = ms;
  //}


  return mp_const_none;
}

// state
mp_obj_t picosystem_pen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_clip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_blend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_target(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_camera(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_spritesheet(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

// primitives
mp_obj_t picosystem_clear() {
    return mp_const_none;
}

mp_obj_t picosystem_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_hline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_vline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_rect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_poly(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_frect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_fcircle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_fpoly(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_line(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_blit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_sprite(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

// utility
mp_obj_t picosystem_str(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_intersects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_intersection(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_contains(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

mp_obj_t picosystem_wrap(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return mp_const_none;
}

// hardware
mp_obj_t picosystem_pressed(mp_obj_t b_obj) {
    bool button_pressed = false;
    
    int b = mp_obj_get_int(b_obj);
    switch(b) {
        case button::UP:
        case button::DOWN:
        case button::LEFT:
        case button::RIGHT:
        case button::A:
        case button::B:
        case button::X:
        case button::Y:
            button_pressed = pressed(b);
            break;

        default:
            mp_raise_ValueError("button not valid.");
            break;
    }

    return button_pressed ? mp_const_true : mp_const_false;
}

mp_obj_t picosystem_button(mp_obj_t b_obj) {
    bool button_down = false;

    int b = mp_obj_get_int(b_obj);
    switch(b) {
        case button::UP:
        case button::DOWN:
        case button::LEFT:
        case button::RIGHT:
        case button::A:
        case button::B:
        case button::X:
        case button::Y:
            button_down = button(b);
            break;

        default:
            mp_raise_ValueError("button not valid.");
            break;
    }

    return button_down ? mp_const_true : mp_const_false;
}

mp_obj_t picosystem_battery() {
    return mp_obj_new_int(battery());
}

mp_obj_t picosystem_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    int r = mp_obj_get_int(r_obj);
    int g = mp_obj_get_int(g_obj);
    int b = mp_obj_get_int(b_obj);

    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    else if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    else if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else
        led(r, g, b);

    return mp_const_none;
}

mp_obj_t picosystem_backlight(mp_obj_t b_obj) {
    int b = mp_obj_get_int(b_obj);

    if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else
        backlight(b);

    return mp_const_none;
}
}
