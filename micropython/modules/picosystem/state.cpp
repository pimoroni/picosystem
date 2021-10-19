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

typedef struct _PicosystemBuffer_obj_t {
    mp_obj_base_t base;
    buffer_t *buffer;
} _PicosystemBuffer_obj_t;

mp_obj_t picosystem_pen(mp_uint_t n_args, const mp_obj_t *args) {
    switch(n_args) {
        case 1: {
            int p = mp_obj_get_int(args[0]);

            if(p < 0 || p > 0xffff) {
                mp_raise_ValueError("p is not a valid pen.");
            }
            else {
                pen(p);
            }
        } break;

        case 3:
        case 4: {
            int r = mp_obj_get_int(args[0]);
            int g = mp_obj_get_int(args[1]);
            int b = mp_obj_get_int(args[2]);

            if(r < 0 || r > 15) {
                mp_raise_ValueError("r out of range. Expected 0 to 15");
            }
            else if(g < 0 || g > 15) {
                mp_raise_ValueError("g out of range. Expected 0 to 15");
            }
            else if(b < 0 || b > 15) {
                mp_raise_ValueError("b out of range. Expected 0 to 15");
            }
            else {
                if(n_args == 4) {
                    int a = mp_obj_get_int(args[3]);
                    if(a < 0 || a > 15) {
                        mp_raise_ValueError("a out of range. Expected 0 to 15");
                    }
                    else {
                        pen(r, g, b, a);
                    }
                }
                else {
                    pen(r, g, b);
                }
            }
        } break;

        default: {
            char *buffer;
            buffer = (char*)malloc(100);
            sprintf(buffer, "function takes 1 (color), 3 (r,g,b), or 4 (r,g,b,a) positional arguments but %d were given", n_args);
            mp_raise_TypeError(buffer);
            free(buffer);
        } break;
    }

    return mp_const_none;
}

mp_obj_t picosystem_clip(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);
    clip(x, y, w, h);
    return mp_const_none;
}

mp_obj_t picosystem_blend(mp_obj_t bf_obj) {
    int bf = mp_obj_get_int(bf_obj);
    switch(bf) {
        case MODE_COPY:
            blend(COPY);
            break;
        case MODE_ALPHA:
            blend(ALPHA);
            break;
        case MODE_MASK:
            blend(MASK);
            break;
        default:
            mp_raise_ValueError("not a valid blend mode. Expected COPY (0), ALPHA (1), or MASK (2)");
    }
    return mp_const_none;
}

mp_obj_t picosystem_target(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        if(mp_obj_is_type(args[0], &PicosystemBuffer_type)) {
            _PicosystemBuffer_obj_t *buffer_obj = MP_OBJ_TO_PTR2(args[0], _PicosystemBuffer_obj_t);
            target(buffer_obj->buffer);
        }
    }
    else {
        target(SCREEN);
    }
    return mp_const_none;
}

mp_obj_t picosystem_camera(mp_obj_t camx_obj, mp_obj_t camy_obj) {
    int camx = mp_obj_get_int(camx_obj);
    int camy = mp_obj_get_int(camy_obj);
    camera(camx, camy);
    return mp_const_none;
}

mp_obj_t picosystem_spritesheet(mp_obj_t ss_obj) {
    if(mp_obj_is_type(ss_obj, &PicosystemBuffer_type)) {
        _PicosystemBuffer_obj_t *buffer_obj = MP_OBJ_TO_PTR2(ss_obj, _PicosystemBuffer_obj_t);
        spritesheet(buffer_obj->buffer);
    }
    else {
        mp_raise_TypeError("spritesheet: not a valid Buffer. Expected a Buffer class");
    }
    return mp_const_none;
}

}