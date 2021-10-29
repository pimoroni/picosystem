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

mp_obj_t picosystem_clear() {
    clear();
    return mp_const_none;
}

mp_obj_t picosystem_pixel(mp_obj_t x_obj, mp_obj_t y_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    pixel(x, y);
    return mp_const_none;
}

mp_obj_t picosystem_rect(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);
    rect(x, y, w, h);
    return mp_const_none;
}

mp_obj_t picosystem_frect(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);
    frect(x, y, w, h);
    return mp_const_none;
}

mp_obj_t picosystem_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int r = mp_obj_get_int(r_obj);
    circle(x, y, r);
    return mp_const_none;
}

mp_obj_t picosystem_fcircle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int r = mp_obj_get_int(r_obj);
    fcircle(x, y, r);
    return mp_const_none;
}

mp_obj_t picosystem_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int rx = mp_obj_get_int(args[2]);
    int ry = mp_obj_get_int(args[3]);
    ellipse(x, y, rx, ry);
    return mp_const_none;
}

mp_obj_t picosystem_fellipse(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int rx = mp_obj_get_int(args[2]);
    int ry = mp_obj_get_int(args[3]);
    fellipse(x, y, rx, ry);
    return mp_const_none;
}

mp_obj_t picosystem_line(mp_uint_t n_args, const mp_obj_t *args) {
    int x1 = mp_obj_get_int(args[0]);
    int y1 = mp_obj_get_int(args[1]);
    int x2 = mp_obj_get_int(args[2]);
    int y2 = mp_obj_get_int(args[3]);
    line(x1, y1, x2, y2);
    return mp_const_none;
}

mp_obj_t picosystem_hline(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t c_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int c = mp_obj_get_int(c_obj);
    hline(x, y, c);
    return mp_const_none;
}

mp_obj_t picosystem_vline(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t c_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int c = mp_obj_get_int(c_obj);
    vline(x, y, c);
    return mp_const_none;
}

void unpack_tuple(mp_obj_t *list, int32_t *points, size_t length) {

}

mp_obj_t picosystem_poly(mp_uint_t n_args, const mp_obj_t *args) {
    size_t num_tuples = n_args;
    const mp_obj_t *tuples = args;

    // Check if there is only one argument, which might be a list
    if(n_args == 1) {
        if(mp_obj_is_type(args[0], &mp_type_list)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(args[0], mp_obj_list_t);
            if(points->len > 0) {
                num_tuples = points->len;
                tuples = points->items;
            }
            else {
                mp_raise_ValueError("poly(): cannot provide an empty list");
            }
        }
        else {
            mp_raise_TypeError("poly(): can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        size_t num_points = num_tuples * 2;
        int32_t* points = new int32_t[num_points];
        size_t i2 = 0;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) {
                mp_raise_ValueError("poly(): can't convert object to tuple");
            }
            else {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);
                if(tuple->len != 2) {
                    mp_raise_ValueError("poly(): tuple must only contain two numbers");
                }
                i2 = i * 2;
                points[i2] = mp_obj_get_int(tuple->items[0]);
                points[i2 + 1] = mp_obj_get_int(tuple->items[1]);
            }
        }
        poly(points, num_tuples);
        delete[] points;
    }

    return mp_const_none;
}

mp_obj_t picosystem_fpoly(mp_uint_t n_args, const mp_obj_t *args) {
    size_t num_tuples = n_args;
    const mp_obj_t *tuples = args;

    // Check if there is only one argument, which might be a list
    if(n_args == 1) {
        if(mp_obj_is_type(args[0], &mp_type_list)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(args[0], mp_obj_list_t);
            if(points->len > 0) {
                num_tuples = points->len;
                tuples = points->items;
            }
            else {
                mp_raise_ValueError("fpoly(): cannot provide an empty list");
            }
        }
        else {
            mp_raise_TypeError("fpoly(): can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        size_t num_points = num_tuples * 2;
        int32_t* points = new int32_t[num_points];
        size_t i2 = 0;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) {
                mp_raise_ValueError("fpoly(): can't convert object to tuple");
            }
            else {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);
                if(tuple->len != 2) {
                    mp_raise_ValueError("fpoly(): tuple must only contain two numbers");
                }
                i2 = i * 2;
                points[i2] = mp_obj_get_int(tuple->items[0]);
                points[i2 + 1] = mp_obj_get_int(tuple->items[1]);
            }
        }
        fpoly(points, num_tuples);
        delete[] points;
    }

    return mp_const_none;
}

mp_obj_t picosystem_blit(mp_uint_t n_args, const mp_obj_t *args) {
    if(mp_obj_is_type(args[0], &PicosystemBuffer_type)) {
        _PicosystemBuffer_obj_t *buffer_obj = MP_OBJ_TO_PTR2(args[0], _PicosystemBuffer_obj_t);

        int x = mp_obj_get_int(args[1]);
        int y = mp_obj_get_int(args[2]);
        int w = mp_obj_get_int(args[3]);
        int h = mp_obj_get_int(args[4]);
        int dx = mp_obj_get_int(args[5]);
        int dy = mp_obj_get_int(args[6]);
        if (n_args == 10) {
            int dw = mp_obj_get_int(args[7]);
            int dh = mp_obj_get_int(args[8]);
            int flags = mp_obj_get_int(args[9]);
            blit(buffer_obj->buffer, x, y, w, h, dx, dy, dw, dh, flags);
        } else if (n_args == 9) {
            int dw = mp_obj_get_int(args[7]);
            int dh = mp_obj_get_int(args[8]);
            blit(buffer_obj->buffer, x, y, w, h, dx, dy, dw, dh);
        } else {
            blit(buffer_obj->buffer, x, y, w, h, dx, dy);
        }
    }
    else {
        mp_raise_TypeError("blit(): src is not a valid buffer. Expected a Buffer class");
    }
    return mp_const_none;
}

// TODO cx, cy, dw and dh should probably be kwargs now?
mp_obj_t picosystem_sprite(mp_uint_t n_args, const mp_obj_t *args) {
    int i = mp_obj_get_int(args[0]);
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    if(n_args == 3) {
        sprite(i, x, y);
    }
    else if(n_args == 5) {
        int cx = mp_obj_get_int(args[3]);
        int cy = mp_obj_get_int(args[4]);
        sprite(i, x, y, cx, cy);
    }
    else if(n_args == 7){
        int cx = mp_obj_get_int(args[3]);
        int cy = mp_obj_get_int(args[4]);
        int dw = mp_obj_get_int(args[5]);
        int dh = mp_obj_get_int(args[6]);
        sprite(i, x, y, cx, cy, dw, dh);
    }
    else if(n_args == 8){
        int cx = mp_obj_get_int(args[3]);
        int cy = mp_obj_get_int(args[4]);
        int dw = mp_obj_get_int(args[5]);
        int dh = mp_obj_get_int(args[6]);
        int flags = mp_obj_get_int(args[7]);
        sprite(i, x, y, cx, cy, dw, dh, flags);
    }
    return mp_const_none;
}

}