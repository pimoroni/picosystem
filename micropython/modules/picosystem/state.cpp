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
        case 0: {
            pen();
        } break;

        case 1: {
            int p = mp_obj_get_int(args[0]);

            if(p < 0 || p > 0xffff) {
                mp_raise_ValueError("pen(): p is not a valid pen.");
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
                mp_raise_ValueError("pen(): r out of range. Expected 0 to 15");
            }
            else if(g < 0 || g > 15) {
                mp_raise_ValueError("pen(): g out of range. Expected 0 to 15");
            }
            else if(b < 0 || b > 15) {
                mp_raise_ValueError("pen(): b out of range. Expected 0 to 15");
            }
            else {
                if(n_args == 4) {
                    int a = mp_obj_get_int(args[3]);
                    if(a < 0 || a > 15) {
                        mp_raise_ValueError("pen(): a out of range. Expected 0 to 15");
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
            __builtin_unreachable();
            break;
        }
    }

    return mp_const_none;
}

mp_obj_t picosystem_alpha(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        int a = mp_obj_get_int(args[0]);
        if(a < 0 || a > 15) {
            mp_raise_ValueError("alpha(): a out of range. Expected 0 to 15");
        } else {
            alpha(a);
        }
    } else {
        alpha();
    }
    return mp_const_none;
}

mp_obj_t picosystem_clip(mp_uint_t n_args, const mp_obj_t *args) {
    switch(n_args) {
        case 4: {
            int x = mp_obj_get_int(args[0]);
            int y = mp_obj_get_int(args[1]);
            int w = mp_obj_get_int(args[2]);
            int h = mp_obj_get_int(args[3]);
            clip(x, y, w, h);
        } break;
        case 0: {
            clip();
        } break;
        default: {
            char *buffer;
            buffer = (char*)malloc(256);
            snprintf(buffer, 256, "clip() takes 0 (reset) or 4 (x, y, w, h) positional arguments but %d were given", n_args);
            mp_raise_TypeError(buffer);
            free(buffer);
        } break;
    }
    return mp_const_none;
}

mp_obj_t picosystem_blend(mp_uint_t n_args, const mp_obj_t *args) {
    if(n_args == 1) {
        int bf = mp_obj_get_int(args[0]);
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
            case MODE_PEN:
                blend(PEN);
                break;
            case MODE_DARKEN:
                blend(DARKEN);
                break;
            case MODE_LIGHTEN:
                blend(LIGHTEN);
                break;
            case MODE_ADD:
                blend(ADD);
                break;
            case MODE_SUBTRACT:
                blend(SUBTRACT);
                break;
            case MODE_MULTIPLY:
                blend(MULTIPLY);
                break;
            case MODE_DISSOLVE:
                blend(DISSOLVE);
                break;
            default:
                mp_raise_ValueError("blend(): not a valid blend mode. Expected: COPY, ALPHA, MASK, PEN, DARKEN, LIGHTEN, ADD, SUBTRACT, MULTIPLY, DISSOLVE");
        }
    } else {
        blend();
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
        target();
    }
    return mp_const_none;
}

mp_obj_t picosystem_camera(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 2) {
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        camera(x, y);
    } else if (n_args == 1) {
        mp_raise_TypeError("camera() takes 0 (reset) or 2 (x, y) positional arguments.");
    } else {
        camera();
    }
    return mp_const_none;
}

mp_obj_t picosystem_cursor(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 2) {
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        cursor(x, y);
    } else if (n_args == 1) {
        mp_raise_TypeError("cursor() takes 0 (reset) or 2 (x, y) positional arguments.");
    } else {
        cursor();
    }
    return mp_const_none;
}

mp_obj_t picosystem_spritesheet(mp_uint_t n_args, const mp_obj_t *args)  {
    if (n_args == 1) {
        if(mp_obj_is_type(args[0], &PicosystemBuffer_type)) {
            _PicosystemBuffer_obj_t *buffer_obj = MP_OBJ_TO_PTR2(args[0], _PicosystemBuffer_obj_t);
            spritesheet(buffer_obj->buffer);
        }
        else {
            mp_raise_TypeError("spritesheet(): not a valid Buffer. Expected a Buffer class");
        }
    } else {
        spritesheet();
    }
    return mp_const_none;
}

}