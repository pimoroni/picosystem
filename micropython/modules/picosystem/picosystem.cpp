#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "libraries/picosystem.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

using namespace picosystem;

extern "C" {
#include "picosystem.h"
#include "math.h"
#include "cstring"

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

static bool done_splash = false;

mp_obj_t update_callback_obj = mp_const_none;
mp_obj_t draw_callback_obj = mp_const_none;

typedef struct _PicosystemBuffer_obj_t {
    mp_obj_base_t base;
    buffer_t *buffer;
} _PicosystemBuffer_obj_t;
  
typedef struct _PicosystemVoice_obj_t {
    mp_obj_base_t base;
    voice_t *voice;
} _PicosystemVoice_obj_t;

mp_obj_t pimoroni_mp_load_global(qstr qst) {
    mp_map_elem_t *elem = mp_map_lookup(&mp_globals_get()->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
    if (elem == NULL) {
        return mp_const_none;
    }
    return elem->value;
}

mp_obj_t picosystem_reset() {
    update_callback_obj = mp_const_none;
    draw_callback_obj = mp_const_none;
    return mp_const_none;
}

mp_obj_t picosystem_init() {

    MP_STATE_PORT(picosystem_framebuffer) = m_new(color_t, 120 * 120);
    SCREEN = buffer(120, 120, MP_STATE_PORT(picosystem_framebuffer));
    target(SCREEN);

    update_callback_obj = mp_const_none;
    draw_callback_obj = mp_const_none;

    _init_hardware();

    // setup lut for fast sin/cos functions
    for(uint32_t i = 0; i < 256; i++) {
        _fsin_lut[i] = sin((_PI * 2.0f) * (float(i) / 256.0f));
    }

    if(!done_splash) { // Squash splash on soft-reset. It's painful!
        _start_audio(); // HACK! we really should figure out *what* soft-reset rp2_pio_deinit and machine_pin_deinit is breaking

        // Set the LED to green, just for gentle reassurance
        led(0, 128, 0);

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
#endif
        done_splash = true;
    }
    backlight(75);

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

    if(update_callback_obj == mp_const_none) {
        update_callback_obj = pimoroni_mp_load_global(qstr_from_str("update"));
        if(update_callback_obj == mp_const_none) {
            //TODO switch out this URL for the final one
            mp_raise_msg(&mp_type_NameError, "a function named 'update(ticks)' is not defined. Check out https://github.com/pimoroni/picosystem-micropython/README.md for instructions");
        }
    }
    if(draw_callback_obj == mp_const_none) {
        draw_callback_obj = mp_load_global(qstr_from_str("draw"));
        if(draw_callback_obj == mp_const_none) {
            //TODO switch out this URL for the final one
            mp_raise_msg(&mp_type_NameError, "a function named 'draw()' is not defined. Check out https://github.com/pimoroni/picosystem-micropython/README.md for instructions");
        }
    }

    //while(true) {
    uint32_t ms = time();

    // work out how many milliseconds of updates we're waiting
    // to process and then call the users update() function as
    // many times as needed to catch up
    pending_update_ms += (ms - last_ms);
    while(pending_update_ms >= update_rate_ms) {
        _lio = _io;
        _io = _gpio_get();

        mp_call_function_1(update_callback_obj, mp_obj_new_int(tick++));
        pending_update_ms -= update_rate_ms;
    }

    // if current flipping the framebuffer in the background
    // then wait until that is complete before allow the user
    // to render
    while(_is_flipping()) {}

    // call user render function to draw world
    mp_call_function_0(draw_callback_obj);

    // wait for the screen to vsync before triggering flip
    // to ensure no tearing
    _wait_vsync();

    // flip the framebuffer to the screen
    _flip();

    last_ms = ms;
    //}


    return mp_const_none;
}


}
