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
bool running = true;

uint32_t tick = 0;

static bool done_audio_init = false;

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
    running = false;
    update_callback_obj = mp_const_none;
    draw_callback_obj = mp_const_none;
    tick = 0;
    return mp_const_none;
}

mp_obj_t picosystem_logo() {
    const uint8_t *s = _picosystem_logo;

    for(int y = 35; y < 85; y++) {
        for(int x = 19; x < 101; x+=8) {
            for(int bit = 0; bit < 8; bit++) {
                if(*s & (0b10000000 >> bit)) {
                    pixel(x + bit, y);
                }
            }
            s++;
        }
    }

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

    if(!done_audio_init) {
        _start_audio(); // HACK! we really should figure out *what* soft-reset rp2_pio_deinit and machine_pin_deinit is breaking

        // Set the LED to green, just for gentle reassurance
        led(0, 64, 0);
    
        done_audio_init = true;
    }

    // Keep the screen off...
    backlight(0);
    // Screen buffer is initialized clear; just flip it.
    _flip();
    // Wait for the DMA transfer to finish
    while (_is_flipping());
    // wait for the screen to update
    _wait_vsync();
    _wait_vsync();

    // Turn the screen on
    backlight(75);

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

mp_obj_t picosystem_run() {
        uint32_t start_tick_us = time_us();

    if(update_callback_obj == mp_const_none) {
        update_callback_obj = pimoroni_mp_load_global(qstr_from_str("update"));
        if(update_callback_obj == mp_const_none) {
            //TODO switch out this URL for the final one
            mp_raise_msg(&mp_type_NameError, "a function named 'update(ticks)' is not defined. Check out https://github.com/pimoroni/picosystem/blob/main/micropython/README.md for instructions");
        }
    }

    if(draw_callback_obj == mp_const_none) {
        draw_callback_obj = mp_load_global(qstr_from_str("draw"));
        if(draw_callback_obj == mp_const_none) {
            //TODO switch out this URL for the final one
            mp_raise_msg(&mp_type_NameError, "a function named 'draw()' is not defined. Check out https://github.com/pimoroni/picosystem/blob/main/micropython/README.md for instructions");
        }
    }

    _io = _gpio_get();
    running = true;

    while(running) {

        // call users update() function
        uint32_t start_update_us = time_us();

        // store previous io state and get new io state
        _lio = _io;
        _io = _gpio_get();

        mp_call_function_1(update_callback_obj, mp_obj_new_int(tick++));
        stats.update_us = time_us() - start_update_us;

        // if we're currently transferring the the framebuffer to the screen then
        // wait until that is complete before allowing the user to do their drawing
        uint32_t wait_us = 0;
        uint32_t start_wait_flip_us = time_us();
        while(_is_flipping()) {
            MICROPY_EVENT_POLL_HOOK
        }
        wait_us += time_us() - start_wait_flip_us;

        // call user render function to draw world
        uint32_t start_draw_us = time_us();
        mp_call_function_1(draw_callback_obj, mp_obj_new_int(tick));
        stats.draw_us = time_us() - start_draw_us;

        // wait for the screen to vsync before triggering flip
        // to ensure no tearing
        uint32_t start_wait_vsync_us = time_us();
        _wait_vsync();
        wait_us += time_us() - start_wait_vsync_us;

        // flip the framebuffer to the screen
        _flip();

        tick++;

        stats.tick_us = time_us() - start_tick_us;

        // calculate fps and round to nearest value (instead of truncating/floor)
        stats.fps = (1000000 - 1) / stats.tick_us + 1;

        if(stats.fps > 40) {
            // if fps is high enough then we definitely didn't miss vsync
            stats.idle = (wait_us * 100) / stats.tick_us;
        }else{
            // if we missed vsync then we overran the frame time and hence had
            // no idle time
            stats.idle = 0;
        }

        MICROPY_EVENT_POLL_HOOK

    }

    update_callback_obj = mp_const_none;
    draw_callback_obj = mp_const_none;
    tick = 0;

    return mp_const_none;
}

mp_obj_t picosystem_quit(){
    running = false;
    return mp_const_none;
}

mp_obj_t picosystem_tick() {
    uint32_t start_tick_us = time_us();

    if(update_callback_obj == mp_const_none) {
        update_callback_obj = pimoroni_mp_load_global(qstr_from_str("update"));
        if(update_callback_obj == mp_const_none) {
            //TODO switch out this URL for the final one
            mp_raise_msg(&mp_type_NameError, "a function named 'update(ticks)' is not defined. Check out https://github.com/pimoroni/picosystem/blob/main/micropython/README.md for instructions");
        }
    }

    if(draw_callback_obj == mp_const_none) {
        draw_callback_obj = mp_load_global(qstr_from_str("draw"));
        if(draw_callback_obj == mp_const_none) {
            //TODO switch out this URL for the final one
            mp_raise_msg(&mp_type_NameError, "a function named 'draw()' is not defined. Check out https://github.com/pimoroni/picosystem/blob/main/micropython/README.md for instructions");
        }
    }

    // store previous io state and get new io state
    _lio = _io;
    _io = _gpio_get();

    // call users update() function
    uint32_t start_update_us = time_us();
    mp_call_function_1(update_callback_obj, mp_obj_new_int(tick++));
    stats.update_us = time_us() - start_update_us;

    // if we're currently transferring the the framebuffer to the screen then
    // wait until that is complete before allowing the user to do their drawing
    uint32_t wait_us = 0;
    uint32_t start_wait_flip_us = time_us();
    while(_is_flipping()) {
        MICROPY_EVENT_POLL_HOOK
    }
    wait_us += time_us() - start_wait_flip_us;

    // call user render function to draw world
    uint32_t start_draw_us = time_us();
    mp_call_function_1(draw_callback_obj, mp_obj_new_int(tick));
    stats.draw_us = time_us() - start_draw_us;

    // wait for the screen to vsync before triggering flip
    // to ensure no tearing
    uint32_t start_wait_vsync_us = time_us();
    _wait_vsync();
    wait_us += time_us() - start_wait_vsync_us;

    // flip the framebuffer to the screen
    _flip();

    tick++;

    stats.tick_us = time_us() - start_tick_us;

    // calculate fps and round to nearest value (instead of truncating/floor)
    stats.fps = (1000000 - 1) / stats.tick_us + 1;

    if(stats.fps > 40) {
      // if fps is high enough then we definitely didn't miss vsync
      stats.idle = (wait_us * 100) / stats.tick_us;
    }else{
      // if we missed vsync then we overran the frame time and hence had
      // no idle time
      stats.idle = 0;
    }

    return mp_const_none;
}


}
