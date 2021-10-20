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

mp_obj_t picosystem_stats() {
    mp_obj_t tuple[5];
    tuple[0] = mp_obj_new_int(stats.fps);
    tuple[1] = mp_obj_new_int(stats.idle);
    tuple[2] = mp_obj_new_int(stats.tick_us);
    tuple[3] = mp_obj_new_int(stats.update_us);
    tuple[4] = mp_obj_new_int(stats.draw_us);
    return mp_obj_new_tuple(5, tuple);
}

}