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

/***** Print *****/
void PicosystemBuffer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PicosystemBuffer_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemBuffer_obj_t);
    mp_print_str(print, "Buffer(");

    mp_print_str(print, "w = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->buffer->w), PRINT_REPR);

    mp_print_str(print, ", h = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->buffer->h), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Destructor ******/
mp_obj_t PicosystemBuffer___del__(mp_obj_t self_in) {
    _PicosystemBuffer_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemBuffer_obj_t);
    uint32_t size = self->buffer->w * self->buffer->h;
    m_del(color_t, self->buffer->data, size);
    m_del(buffer_t, self->buffer, 1);
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t PicosystemBuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PicosystemBuffer_obj_t *self = nullptr;

    enum { ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_w, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_h, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    if(w <= 0 || h <= 0) {
        mp_raise_ValueError("Buffer: both w and h must be greater than zero");
    }

    self = m_new_obj_with_finaliser(_PicosystemBuffer_obj_t);
    self->base.type = &PicosystemBuffer_type;

    self->buffer = m_new(buffer_t, 1);
    self->buffer->w = w;
    self->buffer->h = h;
    uint32_t size = w * h;
    self->buffer->data = m_new(color_t, size);
    //memset(self->buffer.data, 0, sizeof(color_t) * size);

    return MP_OBJ_FROM_PTR(self);
}

mp_int_t PicosystemBuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    _PicosystemBuffer_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemBuffer_obj_t);;
    bufinfo->buf = (void *)(self->buffer->data);
    bufinfo->len = self->buffer->w * self->buffer->h * sizeof(color_t);
    bufinfo->typecode = 'B'; // view PicosystemBuffer as bytes
    return 0;
}

typedef struct _PicosystemVoice_obj_t {
    mp_obj_base_t base;
    voice_t *voice;
} _PicosystemVoice_obj_t;

void PicosystemVoice_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemVoice_obj_t);
    mp_print_str(print, "Voice(");

    mp_print_str(print, "envelope: ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->attack), PRINT_REPR);
    mp_print_str(print, "ms, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->decay), PRINT_REPR);
    mp_print_str(print, "ms, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->sustain), PRINT_REPR);
    mp_print_str(print, "%, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->release), PRINT_REPR);
    mp_print_str(print, "ms");

    mp_print_str(print, ", effects: Reverb ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->reverb), PRINT_REPR);
    mp_print_str(print, "ms, Noise ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->noise), PRINT_REPR);
    mp_print_str(print, "%, Distort ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->distort), PRINT_REPR);
    mp_print_str(print, "%");

    mp_print_str(print, ", bend: ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->bend), PRINT_REPR);
    mp_print_str(print, "Hz, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->bend_ms), PRINT_REPR);
    mp_print_str(print, "ms )");
}

mp_obj_t PicosystemVoice_envelope(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_attack, ARG_decay, ARG_sustain, ARG_release };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_attack, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_decay, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_sustain, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_release, MP_ARG_INT, {.u_int = 0} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->voice->attack    = args[ARG_attack].u_int;
    self->voice->decay     = args[ARG_decay].u_int;
    self->voice->sustain   = args[ARG_sustain].u_int;
    self->voice->release   = args[ARG_release].u_int;

    return mp_const_none;
}

mp_obj_t PicosystemVoice_effects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_reverb, ARG_noise, ARG_distort };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_reverb, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_noise, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_distort, MP_ARG_INT, {.u_int = 0} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->voice->reverb    = args[ARG_reverb].u_int;
    self->voice->noise     = args[ARG_noise].u_int;
    self->voice->distort   = args[ARG_distort].u_int;

    return mp_const_none;
}

mp_obj_t PicosystemVoice_bend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_amount, ARG_speed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_amount, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_speed, MP_ARG_INT, {.u_int = 0} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->voice->bend    = args[ARG_amount].u_int;
    self->voice->bend_ms = args[ARG_speed].u_int;

    return mp_const_none;
}

/* TODO replace with set_adsr set_volume set_effects

void PicosystemVoice_attr_get_set(mp_obj_t *dest, uint32_t *val) {
    if(dest[0] ==  MP_OBJ_NULL) {
        dest[0] = mp_obj_new_int(*val);
    } else if (mp_obj_is_small_int(dest[1])) {
        *val = mp_obj_get_int(dest[1]);
        dest[0] = MP_OBJ_NULL;
    } else {
        mp_raise_msg(&mp_type_ValueError, "ValueError: int required!");
    }
}

void PicosystemVoice_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemVoice_obj_t);

    switch(attr) {
        case MP_QSTR_frequency:
            return PicosystemVoice_attr_get_set(dest, &self->voice->frequency);
        case MP_QSTR_bend:
            return PicosystemVoice_attr_get_set(dest, &self->voice->bend);
        case MP_QSTR_attack:
            return PicosystemVoice_attr_get_set(dest, &self->voice->attack);
        case MP_QSTR_decay:
            return PicosystemVoice_attr_get_set(dest, &self->voice->decay);
        case MP_QSTR_hold:
            return PicosystemVoice_attr_get_set(dest, &self->voice->hold);
        case MP_QSTR_release:
            return PicosystemVoice_attr_get_set(dest, &self->voice->release);
        case MP_QSTR_reverb:
            return PicosystemVoice_attr_get_set(dest, &self->voice->reverb);
        case MP_QSTR_sustain:
            return PicosystemVoice_attr_get_set(dest, &self->voice->sustain);
        case MP_QSTR_volume:
            return PicosystemVoice_attr_get_set(dest, &self->voice->volume);
        case MP_QSTR_noise:
            return PicosystemVoice_attr_get_set(dest, &self->voice->noise);
        case MP_QSTR_distort:
            return PicosystemVoice_attr_get_set(dest, &self->voice->distort);
        default:
            return;
    }
}
*/

mp_obj_t PicosystemVoice___del__(mp_obj_t self_in) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemVoice_obj_t);
    m_del(voice_t, self->voice, 1);
    return mp_const_none;
}

uint32_t note_to_freq(const char *note) {
    switch (qstr_from_str(note)) {
        case MP_QSTR_C0: return 16;
        case MP_QSTR_Cs0:
        case MP_QSTR_Db0: return 17;
        case MP_QSTR_D0: return 18;
        case MP_QSTR_Ds0:
        case MP_QSTR_Eb0: return 19;
        case MP_QSTR_E0: return 20;
        case MP_QSTR_F0: return 21;
        case MP_QSTR_Fs0:
        case MP_QSTR_Gb0: return 23;
        case MP_QSTR_G0: return 24;
        case MP_QSTR_Gs0:
        case MP_QSTR_Ab0: return 25;
        case MP_QSTR_A0: return 27;
        case MP_QSTR_As0:
        case MP_QSTR_Bb0: return 29;
        case MP_QSTR_B0: return 30;
        case MP_QSTR_C1: return 32;
        case MP_QSTR_Cs1:
        case MP_QSTR_Db1: return 34;
        case MP_QSTR_D1: return 36;
        case MP_QSTR_Ds1:
        case MP_QSTR_Eb1: return 38;
        case MP_QSTR_E1: return 41;
        case MP_QSTR_F1: return 43;
        case MP_QSTR_Fs1:
        case MP_QSTR_Gb1: return 46;
        case MP_QSTR_G1: return 49;
        case MP_QSTR_Gs1:
        case MP_QSTR_Ab1: return 51;
        case MP_QSTR_A1: return 55;
        case MP_QSTR_As1:
        case MP_QSTR_Bb1: return 58;
        case MP_QSTR_B1: return 61;
        case MP_QSTR_C2: return 65;
        case MP_QSTR_Cs2:
        case MP_QSTR_Db2: return 69;
        case MP_QSTR_D2: return 73;
        case MP_QSTR_Ds2:
        case MP_QSTR_Eb2: return 77;
        case MP_QSTR_E2: return 82;
        case MP_QSTR_F2: return 87;
        case MP_QSTR_Fs2:
        case MP_QSTR_Gb2: return 92;
        case MP_QSTR_G2: return 98;
        case MP_QSTR_Gs2:
        case MP_QSTR_Ab2: return 103;
        case MP_QSTR_A2: return 110;
        case MP_QSTR_As2:
        case MP_QSTR_Bb2: return 116;
        case MP_QSTR_B2: return 123;
        case MP_QSTR_C3: return 130;
        case MP_QSTR_Cs3:
        case MP_QSTR_Db3: return 138;
        case MP_QSTR_D3: return 146;
        case MP_QSTR_Ds3:
        case MP_QSTR_Eb3: return 155;
        case MP_QSTR_E3: return 164;
        case MP_QSTR_F3: return 174;
        case MP_QSTR_Fs3:
        case MP_QSTR_Gb3: return 185;
        case MP_QSTR_G3: return 196;
        case MP_QSTR_Gs3:
        case MP_QSTR_Ab3: return 207;
        case MP_QSTR_A3: return 220;
        case MP_QSTR_As3:
        case MP_QSTR_Bb3: return 233;
        case MP_QSTR_B3: return 246;
        case MP_QSTR_C4: return 261;
        case MP_QSTR_Cs4:
        case MP_QSTR_Db4: return 277;
        case MP_QSTR_D4: return 293;
        case MP_QSTR_Ds4:
        case MP_QSTR_Eb4: return 311;
        case MP_QSTR_E4: return 329;
        case MP_QSTR_F4: return 349;
        case MP_QSTR_Fs4:
        case MP_QSTR_Gb4: return 369;
        case MP_QSTR_G4: return 392;
        case MP_QSTR_Gs4:
        case MP_QSTR_Ab4: return 415;
        case MP_QSTR_A4: return 440;
        case MP_QSTR_As4:
        case MP_QSTR_Bb4: return 466;
        case MP_QSTR_B4: return 493;
        case MP_QSTR_C5: return 523;
        case MP_QSTR_Cs5:
        case MP_QSTR_Db5: return 554;
        case MP_QSTR_D5: return 587;
        case MP_QSTR_Ds5:
        case MP_QSTR_Eb5: return 622;
        case MP_QSTR_E5: return 659;
        case MP_QSTR_F5: return 698;
        case MP_QSTR_Fs5:
        case MP_QSTR_Gb5: return 739;
        case MP_QSTR_G5: return 783;
        case MP_QSTR_Gs5:
        case MP_QSTR_Ab5: return 830;
        case MP_QSTR_A5: return 880;
        case MP_QSTR_As5:
        case MP_QSTR_Bb5: return 932;
        case MP_QSTR_B5: return 987;
        case MP_QSTR_C6: return 1046;
        case MP_QSTR_Cs6:
        case MP_QSTR_Db6: return 1108;
        case MP_QSTR_D6: return 1174;
        case MP_QSTR_Ds6:
        case MP_QSTR_Eb6: return 1244;
        case MP_QSTR_E6: return 1318;
        case MP_QSTR_F6: return 1396;
        case MP_QSTR_Fs6:
        case MP_QSTR_Gb6: return 1479;
        case MP_QSTR_G6: return 1567;
        case MP_QSTR_Gs6:
        case MP_QSTR_Ab6: return 1661;
        case MP_QSTR_A6: return 1760;
        case MP_QSTR_As6:
        case MP_QSTR_Bb6: return 1864;
        case MP_QSTR_B6: return 1975;
        case MP_QSTR_C7: return 2093;
        case MP_QSTR_Cs7:
        case MP_QSTR_Db7: return 2217;
        case MP_QSTR_D7: return 2349;
        case MP_QSTR_Ds7:
        case MP_QSTR_Eb7: return 2489;
        case MP_QSTR_E7: return 2637;
        case MP_QSTR_F7: return 2793;
        case MP_QSTR_Fs7:
        case MP_QSTR_Gb7: return 2959;
        case MP_QSTR_G7: return 3135;
        case MP_QSTR_Gs7:
        case MP_QSTR_Ab7: return 3322;
        case MP_QSTR_A7: return 3520;
        case MP_QSTR_As7:
        case MP_QSTR_Bb7: return 3729;
        case MP_QSTR_B7: return 3951;
        case MP_QSTR_C8: return 4186;
        case MP_QSTR_Cs8:
        case MP_QSTR_Db8: return 4434;
        case MP_QSTR_D8: return 4698;
        case MP_QSTR_Ds8:
        case MP_QSTR_Eb8: return 4978;
        case MP_QSTR_E8: return 5274;
        case MP_QSTR_F8: return 5587;
        case MP_QSTR_Fs8:
        case MP_QSTR_Gb8: return 5919;
        case MP_QSTR_G8: return 6271;
        case MP_QSTR_Gs8:
        case MP_QSTR_Ab8: return 6644;
        case MP_QSTR_A8: return 7040;
        case MP_QSTR_As8:
        case MP_QSTR_Bb8: return 7458;
        case MP_QSTR_B8: return 7902;
        default: return 0;
    }
}

mp_obj_t PicosystemVoice_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_note, ARG_duration, ARG_volume };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_note, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_duration, MP_ARG_INT, {.u_int = 500} },
        { MP_QSTR_volume, MP_ARG_INT, {.u_int = 100} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t freq = 0;

    mp_obj_t dt_obj = args[ARG_note].u_obj;
    if(mp_obj_is_small_int(dt_obj)) {
        freq = mp_obj_get_int(dt_obj);
    } else if (mp_obj_is_str_or_bytes(dt_obj)) {
        GET_STR_DATA_LEN(dt_obj, str, str_len);
        freq = note_to_freq((const char*)str);
    }

    if(freq == 0) {
        mp_raise_TypeError("play: note must be a frequency (int) or note name (eg: G8)");
    }
    
    play(*(self->voice), freq, args[ARG_duration].u_int, args[ARG_volume].u_int);
    return mp_const_none;
}

mp_obj_t PicosystemVoice_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PicosystemVoice_obj_t *self = nullptr;

    enum {ARG_attack, ARG_decay, ARG_sustain, ARG_release, ARG_bend, ARG_bend_ms, ARG_reverb, ARG_noise, ARG_distort };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_attack, MP_ARG_INT, {.u_int = 100} },
        { MP_QSTR_decay, MP_ARG_INT, {.u_int = 50} },
        { MP_QSTR_sustain, MP_ARG_INT, {.u_int = 80} },
        { MP_QSTR_release, MP_ARG_INT, {.u_int = 100} },

        { MP_QSTR_bend, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bend_ms, MP_ARG_INT, {.u_int = 0} },
    
        { MP_QSTR_reverb, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_noise, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_distort, MP_ARG_INT, {.u_int = 0} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj_with_finaliser(_PicosystemVoice_obj_t);
    self->base.type = &PicosystemVoice_type;

    self->voice = m_new(voice_t, 1);
    self->voice->attack    = args[ARG_attack].u_int;
    self->voice->decay     = args[ARG_decay].u_int;
    self->voice->sustain   = args[ARG_sustain].u_int;
    self->voice->release   = args[ARG_release].u_int;

    self->voice->bend      = args[ARG_bend].u_int;
    self->voice->bend_ms   = args[ARG_bend_ms].u_int;

    self->voice->reverb    = args[ARG_reverb].u_int;
    self->voice->noise     = args[ARG_noise].u_int;
    self->voice->distort   = args[ARG_distort].u_int;

    return MP_OBJ_FROM_PTR(self);
}

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

// state
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

mp_obj_t picosystem_audio_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(!mp_obj_is_type(pos_args[0], &PicosystemVoice_type)) {
        mp_raise_TypeError("play: not a valid Voice. Expected a Voice class");
    }

    enum { ARG_duration, ARG_volume };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_duration, MP_ARG_INT, {.u_int = 500} },
        { MP_QSTR_volume, MP_ARG_INT, {.u_int = 100} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t freq = 0;

    mp_obj_t dt_obj = pos_args[1];
    if(mp_obj_is_small_int(dt_obj)) {
        freq = mp_obj_get_int(dt_obj);
    } else if (mp_obj_is_str_or_bytes(dt_obj)) {
        GET_STR_DATA_LEN(dt_obj, str, str_len);
        freq = note_to_freq((const char*)str);
    }

    if(freq == 0) {
        mp_raise_TypeError("play: note must be a frequency (int) or note name (eg: G8)");
    }

    _PicosystemVoice_obj_t *voice = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);

    play(*(voice->voice), freq, args[ARG_duration].u_int, args[ARG_volume].u_int);

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

// primitives
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

mp_obj_t picosystem_rect(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);
    rect(x, y, w, h);
    return mp_const_none;
}

mp_obj_t picosystem_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int r = mp_obj_get_int(r_obj);
    circle(x, y, r);
    return mp_const_none;
}

mp_obj_t picosystem_poly(mp_uint_t n_args, const mp_obj_t *args) {
    size_t num_tuples = n_args;
    const mp_obj_t *tuples = args;

    // Check if there is only one argument, which might be a list
    if(n_args == 1) {
        if(mp_obj_is_type(args[0], &PicosystemBuffer_type)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(args[0], mp_obj_list_t);
            if(points->len > 0) {
                num_tuples = points->len;
                tuples = points->items;
            }
            else {
                mp_raise_ValueError("cannot provide an empty list");
            }
        }
        else {
            mp_raise_TypeError("can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        size_t num_points = num_tuples * 2;
        int32_t* points = new int32_t[num_points];
        size_t i2 = 0;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) {
                mp_raise_ValueError("can't convert object to tuple");
            }
            else {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);
                if(tuple->len != 2) {
                    mp_raise_ValueError("tuple must only contain two numbers");
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

mp_obj_t picosystem_frect(mp_uint_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);
    frect(x, y, w, h);
    return mp_const_none;
}

mp_obj_t picosystem_fcircle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int r = mp_obj_get_int(r_obj);
    fcircle(x, y, r);
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
                mp_raise_ValueError("cannot provide an empty list");
            }
        }
        else {
            mp_raise_TypeError("can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        size_t num_points = num_tuples * 2;
        int32_t* points = new int32_t[num_points];
        size_t i2 = 0;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) {
                mp_raise_ValueError("can't convert object to tuple");
            }
            else {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);
                if(tuple->len != 2) {
                    mp_raise_ValueError("tuple must only contain two numbers");
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

mp_obj_t picosystem_line(mp_uint_t n_args, const mp_obj_t *args) {
    int x1 = mp_obj_get_int(args[0]);
    int y1 = mp_obj_get_int(args[1]);
    int x2 = mp_obj_get_int(args[2]);
    int y2 = mp_obj_get_int(args[3]);
    line(x1, y1, x2, y2);
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
        blit(buffer_obj->buffer, x, y, w, h, dx, dy);
    }
    else {
        mp_raise_TypeError("src is not a valid buffer. Expected a Buffer class");
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
    return mp_const_none;
}

mp_obj_t picosystem_text(mp_uint_t n_args, const mp_obj_t *args) {
    if(mp_obj_is_str_or_bytes(args[0])) {
        GET_STR_DATA_LEN(args[0], str, str_len);

        std::string t((const char*)str);

        if(n_args == 3) {
            int x = mp_obj_get_int(args[1]);
            int y = mp_obj_get_int(args[2]);
            text(t, x, y);
        }
        else {
            text(t);
        }
    }
    else if(mp_obj_is_float(args[0])) {
        mp_raise_TypeError("can't convert 'float' object to str implicitly");
    }
    else if(mp_obj_is_int(args[0])) {
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    }
    else if(mp_obj_is_bool(args[0])) {
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    }
    else {
        mp_raise_TypeError("can't convert object to str implicitly");
    }

    return mp_const_none;
}

mp_obj_t picosystem_text_width(mp_obj_t str_obj) {
    if(mp_obj_is_str_or_bytes(str_obj)) {
        GET_STR_DATA_LEN(str_obj, str, str_len);

        std::string t((const char*)str);

        return mp_obj_new_int(text_width(t));
    }
    else if(mp_obj_is_float(str_obj)) {
        mp_raise_TypeError("can't convert 'float' object to str implicitly");
    }
    else if(mp_obj_is_int(str_obj)) {
        mp_raise_TypeError("can't convert 'int' object to str implicitly");
    }
    else if(mp_obj_is_bool(str_obj)) {
        mp_raise_TypeError("can't convert 'bool' object to str implicitly");
    }
    else {
        mp_raise_TypeError("can't convert object to str implicitly");
    }

    return mp_const_none;
}

// utility
mp_obj_t picosystem_rgb(mp_uint_t n_args, const mp_obj_t *args) {
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
        mp_raise_ValueError("h out of range. Expected 0.0 to 1.0");
    }
    else if(s < 0.0f || s > 1.0f) {
        mp_raise_ValueError("s out of range. Expected 0.0 to 1.0");
    }
    else if(v < 0.0f || v > 1.0f) {
        mp_raise_ValueError("v out of range. Expected 0.0 to 1.0");
    }
    else {
        if(n_args == 4) {
            float a = mp_obj_get_float(args[3]);
            if(a < 0.0f || a > 1.0f) {
                mp_raise_ValueError("v out of range. Expected 0.0 to 1.0");
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
            mp_raise_ValueError("button not valid.");
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
            mp_raise_ValueError("button not valid.");
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
        mp_raise_ValueError("r out of range. Expected 0 to 100");
    }
    else if(g < 0 || g > 100) {
        mp_raise_ValueError("g out of range. Expected 0 to 100");
    }
    else if(b < 0 || b > 100) {
        mp_raise_ValueError("b out of range. Expected 0 to 100");
    }
    else {
        led(r, g, b);
    }

    return mp_const_none;
}

mp_obj_t picosystem_backlight(mp_obj_t b_obj) {
    int b = mp_obj_get_int(b_obj);

    if(b < 0 || b > 100) {
        mp_raise_ValueError("b out of range. Expected 0 to 100");
    }
    else {
        backlight(b);
    }

    return mp_const_none;
}
}
