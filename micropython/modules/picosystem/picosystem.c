#include "picosystem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// PicosystemBuffer Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(PicosystemBuffer___del___obj, PicosystemBuffer___del__);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t PicosystemBuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PicosystemBuffer___del___obj) },
};

MP_DEFINE_CONST_DICT(PicosystemBuffer_locals_dict, PicosystemBuffer_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t PicosystemBuffer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Buffer,
    .print = PicosystemBuffer_print,
    .make_new = PicosystemBuffer_make_new,
    .buffer_p = { .get_buffer = PicosystemBuffer_get_buffer },
    .locals_dict = (mp_obj_dict_t*)&PicosystemBuffer_locals_dict,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// PicosystemVoice Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(PicosystemVoice___del___obj, PicosystemVoice___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(PicosystemVoice_play_obj, 2, PicosystemVoice_play);
MP_DEFINE_CONST_FUN_OBJ_KW(PicosystemVoice_envelope_obj, 1, PicosystemVoice_envelope);
MP_DEFINE_CONST_FUN_OBJ_KW(PicosystemVoice_effects_obj, 1, PicosystemVoice_effects);
MP_DEFINE_CONST_FUN_OBJ_KW(PicosystemVoice_bend_obj, 1, PicosystemVoice_bend);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t PicosystemVoice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PicosystemVoice___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&PicosystemVoice_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_envelope), MP_ROM_PTR(&PicosystemVoice_envelope_obj) },
    { MP_ROM_QSTR(MP_QSTR_effects), MP_ROM_PTR(&PicosystemVoice_effects_obj) },
    { MP_ROM_QSTR(MP_QSTR_bend), MP_ROM_PTR(&PicosystemVoice_bend_obj) },
};

MP_DEFINE_CONST_DICT(PicosystemVoice_locals_dict, PicosystemVoice_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t PicosystemVoice_type = {
    { &mp_type_type },
    .name = MP_QSTR_Voice,
    .print = PicosystemVoice_print,
    .make_new = PicosystemVoice_make_new,
    .locals_dict = (mp_obj_dict_t*)&PicosystemVoice_locals_dict,
    //.attr = PicosystemVoice_attr,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// picosystem Module
////////////////////////////////////////////////////////////////////////////////////////////////////


/***** Module Functions *****/

// picosystem.cpp

MP_DEFINE_CONST_FUN_OBJ_0(picosystem_init_obj, picosystem_init);
MP_DEFINE_CONST_FUN_OBJ_0(picosystem_logo_obj, picosystem_logo);
MP_DEFINE_CONST_FUN_OBJ_0(picosystem_start_obj, picosystem_start);
MP_DEFINE_CONST_FUN_OBJ_0(picosystem_quit_obj, picosystem_quit);
MP_DEFINE_CONST_FUN_OBJ_0(picosystem_flip_obj, picosystem_flip);

// stats.cpp

MP_DEFINE_CONST_FUN_OBJ_0(picosystem_stats_obj, picosystem_stats);

// voice.cpp

MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_play_obj, 2, picosystem_audio_play);

// state.cpp

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_pen_obj, 0, 4, picosystem_pen);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_alpha_obj, 0, 4, picosystem_alpha);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_clip_obj, 0, 4, picosystem_clip);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_blend_obj, 0, 1, picosystem_blend);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_target_obj, 0, 1, picosystem_target);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_camera_obj, 0, 2, picosystem_camera);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_cursor_obj, 0, 2, picosystem_cursor);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_spritesheet_obj, 0, 1, picosystem_spritesheet);
// TODO font?

// primitives.cpp

MP_DEFINE_CONST_FUN_OBJ_2(picosystem_pixel_obj, picosystem_pixel);
MP_DEFINE_CONST_FUN_OBJ_3(picosystem_hline_obj, picosystem_hline);
MP_DEFINE_CONST_FUN_OBJ_3(picosystem_vline_obj, picosystem_vline);

MP_DEFINE_CONST_FUN_OBJ_0(picosystem_clear_obj, picosystem_clear);

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_rect_obj, 4, 4, picosystem_rect);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_frect_obj, 4, 4, picosystem_frect);

MP_DEFINE_CONST_FUN_OBJ_3(picosystem_circle_obj, picosystem_circle);
MP_DEFINE_CONST_FUN_OBJ_3(picosystem_fcircle_obj, picosystem_fcircle);

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_ellipse_obj, 4, 4, picosystem_ellipse);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_fellipse_obj, 4, 4, picosystem_fellipse);

MP_DEFINE_CONST_FUN_OBJ_VAR(picosystem_poly_obj, 1, picosystem_poly);
MP_DEFINE_CONST_FUN_OBJ_VAR(picosystem_fpoly_obj, 1, picosystem_fpoly);

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_line_obj, 4, 4, picosystem_line);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_blit_obj, 7, 10, picosystem_blit);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_sprite_obj, 3, 8, picosystem_sprite);

// text.cpp

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_text_obj, 1, 4, picosystem_text);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_measure_obj, 1, 2, picosystem_measure);

// utility.cpp

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_rgb_obj, 3, 4, picosystem_rgb);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_hsv_obj, 3, 4, picosystem_hsv);

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_intersects_obj, 8, 8, picosystem_intersects);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_intersection_obj, 8, 8, picosystem_intersection);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picosystem_contains_obj, 6, 6, picosystem_contains);

// hardware.cpp

MP_DEFINE_CONST_FUN_OBJ_1(picosystem_pressed_obj, picosystem_pressed);
MP_DEFINE_CONST_FUN_OBJ_1(picosystem_button_obj, picosystem_button);
MP_DEFINE_CONST_FUN_OBJ_0(picosystem_battery_obj, picosystem_battery);
MP_DEFINE_CONST_FUN_OBJ_3(picosystem_led_obj, picosystem_led);
MP_DEFINE_CONST_FUN_OBJ_1(picosystem_backlight_obj, picosystem_backlight);

/***** Globals Table *****/
STATIC const mp_map_elem_t picosystem_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picosystem) },
};
MP_DEFINE_CONST_DICT(mp_module_picosystem_globals, picosystem_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picosystem_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picosystem_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picosystem, picosystem_user_cmodule);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


