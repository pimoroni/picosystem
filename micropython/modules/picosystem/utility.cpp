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

// utility
mp_obj_t picosystem_rgb(mp_uint_t n_args, const mp_obj_t *args) {
    int r = mp_obj_get_int(args[0]);
    int g = mp_obj_get_int(args[1]);
    int b = mp_obj_get_int(args[2]);
    if(r < 0 || r > 15) {
        mp_raise_ValueError("rgb(): r out of range. Expected 0 to 15");
    }
    else if(g < 0 || g > 15) {
        mp_raise_ValueError("rgb(): g out of range. Expected 0 to 15");
    }
    else if(b < 0 || b > 15) {
        mp_raise_ValueError("rgb(): b out of range. Expected 0 to 15");
    }
    else {
        if(n_args == 4) {
            int a = mp_obj_get_int(args[3]);
            if(a < 0 || a > 15) {
                mp_raise_ValueError("rgb(): a out of range. Expected 0 to 15");
            }
            else {
                return mp_obj_new_int(rgb(r, g, b, a));
            }
        }
        else {
            return mp_obj_new_int(rgb(r, g, b));
        }
    }
    return mp_obj_new_int(0);
}

mp_obj_t picosystem_hsv(mp_uint_t n_args, const mp_obj_t *args) {
    float h = mp_obj_get_float(args[0]);
    float s = mp_obj_get_float(args[1]);
    float v = mp_obj_get_float(args[2]);
    if(h < 0.0f || h > 1.0f) {
        mp_raise_ValueError("hsv(): h out of range. Expected 0.0 to 1.0");
    }
    else if(s < 0.0f || s > 1.0f) {
        mp_raise_ValueError("hsv(): s out of range. Expected 0.0 to 1.0");
    }
    else if(v < 0.0f || v > 1.0f) {
        mp_raise_ValueError("hsv(): v out of range. Expected 0.0 to 1.0");
    }
    else {
        if(n_args == 4) {
            float a = mp_obj_get_float(args[3]);
            if(a < 0.0f || a > 1.0f) {
                mp_raise_ValueError("hsv(): v out of range. Expected 0.0 to 1.0");
            }
            return mp_obj_new_int(hsv(h, s, v, a));
        }
        else {
            return mp_obj_new_int(hsv(h, s, v));
        }
    }
    return mp_obj_new_int(0);
}

mp_obj_t picosystem_intersects(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);
    int cx = mp_obj_get_int(args[4]);
    int cy = mp_obj_get_int(args[5]);
    int cw = mp_obj_get_int(args[6]);
    int ch = mp_obj_get_int(args[7]);
    return mp_obj_new_bool(intersects(x, y, w, h, cx, cy, cw, ch));
}

mp_obj_t picosystem_intersection(mp_uint_t n_args, const mp_obj_t *args) {
    int32_t x = mp_obj_get_int(args[0]);
    int32_t y = mp_obj_get_int(args[1]);
    int32_t w = mp_obj_get_int(args[2]);
    int32_t h = mp_obj_get_int(args[3]);
    int cx = mp_obj_get_int(args[4]);
    int cy = mp_obj_get_int(args[5]);
    int cw = mp_obj_get_int(args[6]);
    int ch = mp_obj_get_int(args[7]);

    intersection(x, y, w, h, cx, cy, cw, ch);

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(x);
    tuple[1] = mp_obj_new_int(y);
    tuple[2] = mp_obj_new_int(w);
    tuple[3] = mp_obj_new_int(h);
    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t picosystem_contains(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int cx = mp_obj_get_int(args[2]);
    int cy = mp_obj_get_int(args[3]);
    int cw = mp_obj_get_int(args[4]);
    int ch = mp_obj_get_int(args[5]);
    return mp_obj_new_bool(contains(x, y, cx, cy, cw, ch));
}

}