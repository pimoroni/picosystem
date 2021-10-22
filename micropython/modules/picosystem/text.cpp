#include "libraries/picosystem.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

using namespace picosystem;

extern "C" {
#include "picosystem.h"
#include "math.h"
#include "cstring"

mp_obj_t picosystem_text(mp_uint_t n_args, const mp_obj_t *args) {
    if(mp_obj_is_str_or_bytes(args[0])) {
        GET_STR_DATA_LEN(args[0], str, str_len);

        std::string t((const char*)str);

        if(n_args == 4) {
            int x = mp_obj_get_int(args[1]);
            int y = mp_obj_get_int(args[2]);
            int w = mp_obj_get_int(args[3]);
            text(t, x, y, w);
        }
        else if(n_args == 3) {
            int x = mp_obj_get_int(args[1]);
            int y = mp_obj_get_int(args[2]);
            text(t, x, y);
        }
        else if(n_args == 2) {
            int w = mp_obj_get_int(args[1]);
            text(t, w);
        }
        else {
            text(t);
        }
    }
    else {
        mp_raise_TypeError("text(): string required!");
    }

    return mp_const_none;
}

mp_obj_t picosystem_measure(mp_uint_t n_args, const mp_obj_t *args) {
    mp_obj_t str_obj = args[0];
    if(mp_obj_is_str_or_bytes(str_obj)) {
        GET_STR_DATA_LEN(str_obj, str, str_len);

        std::string t((const char*)str);
        int32_t w = 0;
        int32_t h = 0;
        int32_t wrap = n_args == 2 ? mp_obj_get_int(args[1]) : -1;

        measure(t, w, h, wrap);

        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(w);
        tuple[1] = mp_obj_new_int(h);
        return mp_obj_new_tuple(2, tuple);
    }
    else {
        mp_raise_TypeError("text(): string required!");
    }

    return mp_const_none;
}

}