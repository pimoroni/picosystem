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
  
typedef struct _PicosystemVoice_obj_t {
    mp_obj_base_t base;
    voice_t *voice;
} _PicosystemVoice_obj_t;

void PicosystemVoice_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; // Unused input parameter
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemVoice_obj_t);
    mp_print_str(print, "Voice(");

    mp_print_str(print, "\nenvelope: ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->attack), PRINT_REPR);
    mp_print_str(print, "ms, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->decay), PRINT_REPR);
    mp_print_str(print, "ms, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->sustain), PRINT_REPR);
    mp_print_str(print, "%, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->release), PRINT_REPR);
    mp_print_str(print, "ms,");

    mp_print_str(print, "\neffects: Reverb ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->reverb), PRINT_REPR);
    mp_print_str(print, "ms, Noise ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->noise), PRINT_REPR);
    mp_print_str(print, "%, Distort ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->distort), PRINT_REPR);
    mp_print_str(print, "%,");

    mp_print_str(print, "\nbend: ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->bend), PRINT_REPR);
    mp_print_str(print, "Hz, ");
    mp_obj_print_helper(print, mp_obj_new_int(self->voice->bend_ms), PRINT_REPR);
    mp_print_str(print, "ms )");
}

mp_obj_t PicosystemVoice_envelope(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_attack, ARG_decay, ARG_sustain, ARG_release };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_attack, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_decay, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sustain, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_release, MP_ARG_INT, {.u_int = -1} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(args[ARG_attack].u_int > -1) self->voice->attack = args[ARG_attack].u_int;
    if(args[ARG_decay].u_int > -1) self->voice->decay  = args[ARG_decay].u_int;
    if(args[ARG_sustain].u_int > -1) self->voice->sustain = args[ARG_sustain].u_int;
    if(args[ARG_release].u_int > -1) self->voice->release = args[ARG_release].u_int;

    return mp_const_none;
}

mp_obj_t PicosystemVoice_effects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_reverb, ARG_noise, ARG_distort };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_reverb, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_noise, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_distort, MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(args[ARG_reverb].u_int > -1) self->voice->reverb = args[ARG_reverb].u_int;
    if(args[ARG_noise].u_int > -1) self->voice->noise = args[ARG_noise].u_int;
    if(args[ARG_distort].u_int > -1) self->voice->distort = args[ARG_distort].u_int;

    return mp_const_none;
}

mp_obj_t PicosystemVoice_bend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    _PicosystemVoice_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _PicosystemVoice_obj_t);
    
    enum { ARG_amount, ARG_speed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_amount, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_speed, MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->voice->bend    = args[ARG_amount].u_int;
    if(args[ARG_speed].u_int > -1) self->voice->bend_ms = args[ARG_speed].u_int;

    return mp_const_none;
}

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

mp_obj_t picosystem_audio_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(!mp_obj_is_type(pos_args[0], &PicosystemVoice_type)) {
        mp_raise_TypeError("play: not a valid Voice. Expected a Voice class");
    }

    enum { ARG_duration, ARG_volume };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_duration, MP_ARG_INT, {.u_int = 500} },
        { MP_QSTR_volume, MP_ARG_INT, {.u_int = 100} },
    };

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

}
