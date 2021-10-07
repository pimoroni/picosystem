#include "picosystem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// picosystem Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Module Functions *****/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picosystem_init_obj, picosystem_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picosystem_tick_obj, picosystem_tick);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_pen_obj, 1, picosystem_pen);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_clip_obj, 1, picosystem_clip);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_blend_obj, 1, picosystem_blend);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_target_obj, 1, picosystem_target);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_camera_obj, 1, picosystem_camera);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_spritesheet_obj, 1, picosystem_spritesheet);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(picosystem_clear_obj, picosystem_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_pixel_obj, 1, picosystem_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_hline_obj, 1, picosystem_hline);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_vline_obj, 1, picosystem_vline);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_rect_obj, 1, picosystem_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_circle_obj, 1, picosystem_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_poly_obj, 1, picosystem_poly);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_frect_obj, 1, picosystem_frect);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_fcircle_obj, 1, picosystem_fcircle);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_fpoly_obj, 1, picosystem_fpoly);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_line_obj, 1, picosystem_line);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_blit_obj, 1, picosystem_blit);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_sprite_obj, 1, picosystem_sprite);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_text_obj, 1, picosystem_text);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_str_obj, 1, picosystem_str);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_rgb_obj, 1, picosystem_rgb);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_hsv_obj, 1, picosystem_hsv);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_intersects_obj, 1, picosystem_intersects);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_intersection_obj, 1, picosystem_intersection);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_contains_obj, 1, picosystem_contains);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(picosystem_wrap_obj, 1, picosystem_wrap);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(picosystem_pressed_obj, picosystem_pressed);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picosystem_button_obj, picosystem_button);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(picosystem_battery_obj, picosystem_battery);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(picosystem_led_obj, picosystem_led);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(picosystem_backlight_obj, picosystem_backlight);

/***** Globals Table *****/
STATIC const mp_map_elem_t picosystem_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picosystem) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&picosystem_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_tick), MP_ROM_PTR(&picosystem_tick_obj) },

    { MP_ROM_QSTR(MP_QSTR_pen), MP_ROM_PTR(&picosystem_pen_obj) },
    { MP_ROM_QSTR(MP_QSTR_clip), MP_ROM_PTR(&picosystem_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_blend), MP_ROM_PTR(&picosystem_blend_obj) },
    { MP_ROM_QSTR(MP_QSTR_target), MP_ROM_PTR(&picosystem_target_obj) },
    { MP_ROM_QSTR(MP_QSTR_camera), MP_ROM_PTR(&picosystem_camera_obj) },
    { MP_ROM_QSTR(MP_QSTR_spritesheet), MP_ROM_PTR(&picosystem_spritesheet_obj) },

    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picosystem_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&picosystem_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&picosystem_hline_obj) },
    { MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&picosystem_vline_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&picosystem_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&picosystem_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&picosystem_poly_obj) },
    { MP_ROM_QSTR(MP_QSTR_frect), MP_ROM_PTR(&picosystem_frect_obj) },
    { MP_ROM_QSTR(MP_QSTR_fcircle), MP_ROM_PTR(&picosystem_fcircle_obj) },
    { MP_ROM_QSTR(MP_QSTR_fpoly), MP_ROM_PTR(&picosystem_fpoly_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&picosystem_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&picosystem_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_sprite), MP_ROM_PTR(&picosystem_sprite_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&picosystem_text_obj) },

    { MP_ROM_QSTR(MP_QSTR_str), MP_ROM_PTR(&picosystem_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgb), MP_ROM_PTR(&picosystem_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_hsv), MP_ROM_PTR(&picosystem_hsv_obj) },

    { MP_ROM_QSTR(MP_QSTR_intersects), MP_ROM_PTR(&picosystem_intersects_obj) },
    { MP_ROM_QSTR(MP_QSTR_intersection), MP_ROM_PTR(&picosystem_intersection_obj) },
    { MP_ROM_QSTR(MP_QSTR_contains), MP_ROM_PTR(&picosystem_contains_obj) },
    { MP_ROM_QSTR(MP_QSTR_wrap), MP_ROM_PTR(&picosystem_wrap_obj) },

    { MP_ROM_QSTR(MP_QSTR_pressed), MP_ROM_PTR(&picosystem_pressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_button), MP_ROM_PTR(&picosystem_button_obj) },
    { MP_ROM_QSTR(MP_QSTR_battery), MP_ROM_PTR(&picosystem_battery_obj) },
    { MP_ROM_QSTR(MP_QSTR_led), MP_ROM_PTR(&picosystem_led_obj) },
    { MP_ROM_QSTR(MP_QSTR_backlight), MP_ROM_PTR(&picosystem_backlight_obj) },

    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_INT(UP) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_INT(DOWN) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_INT(LEFT) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_INT(RIGHT) },
    { MP_ROM_QSTR(MP_QSTR_A), MP_ROM_INT(A) },
    { MP_ROM_QSTR(MP_QSTR_B), MP_ROM_INT(B) },
    { MP_ROM_QSTR(MP_QSTR_X), MP_ROM_INT(X) },
    { MP_ROM_QSTR(MP_QSTR_Y), MP_ROM_INT(Y) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_picosystem_globals, picosystem_globals_table);

/***** Module Definition *****/
const mp_obj_module_t picosystem_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_picosystem_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
MP_REGISTER_MODULE(MP_QSTR_picosystem, picosystem_user_cmodule, MODULE_PICOSYSTEM_ENABLED);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
