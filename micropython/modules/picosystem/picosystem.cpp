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
#include "py/gc.h"


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

mp_obj_t picosystem_init() {

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
    _wait_vsync(); // Need to wait just a little longer to avoid a tiny flash of garbage

    // Turn the screen on
    backlight(75);

    tick = 0;

    _io = _gpio_get();

    return mp_const_none;
}

mp_obj_t picosystem_start() {
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
        uint32_t start_tick_us = time_us();

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
            best_effort_wfe_or_timeout(make_timeout_time_us(500));
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

        // force a per-frame gc.collect() to avoid stutter
        // cost on the order of microseconds.
        gc_collect();

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

/* quit() - Break out of the main loop started by start() */
mp_obj_t picosystem_quit(){
    running = false;
    return mp_const_none;
}

/* flip() - Flip the buffer, use this from the repl! (or, your own main loop?) */
mp_obj_t picosystem_flip() {
    _lio = _io;
    _io = _gpio_get();

    while(_is_flipping()) {
        best_effort_wfe_or_timeout(make_timeout_time_us(500));
    }
    _wait_vsync();
    _flip();
    return mp_const_none;
}

/* _logo() - Render the 1-bit logo to the center of the screen in the pen colour */
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

}
