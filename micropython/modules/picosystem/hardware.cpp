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
            mp_raise_ValueError("pressed(): invalid button.");
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
            mp_raise_ValueError("button(): invalid button.");
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

    if(r < 0 || r > 100) {
        mp_raise_ValueError("led(): r out of range. Expected 0 to 100");
    }
    else if(g < 0 || g > 100) {
        mp_raise_ValueError("led(): g out of range. Expected 0 to 100");
    }
    else if(b < 0 || b > 100) {
        mp_raise_ValueError("led(): b out of range. Expected 0 to 100");
    }
    else {
        led(r, g, b);
    }

    return mp_const_none;
}

mp_obj_t picosystem_backlight(mp_obj_t b_obj) {
    int b = mp_obj_get_int(b_obj);

    if(b < 0 || b > 100) {
        mp_raise_ValueError("led(): b out of range. Expected 0 to 100");
    }
    else {
        backlight(b);
    }

    return mp_const_none;
}

}