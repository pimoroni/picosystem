// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// input pins
enum button_pin {
    UP    = 23,
    DOWN  = 20,
    LEFT  = 22,
    RIGHT = 21,
    A     = 18,
    B     = 19,
    X     = 17,
    Y     = 16
};

enum blend_mode {
    MODE_COPY = 0,
    MODE_ALPHA,
    MODE_MASK
};

/*
enum _sprites_ids {
    CHERRY, APPLE, BANANA, ORANGE, AUBERGINE, CARROT, POTATO, LEAF,
    PARSNIP, MEAT, BEAN, RADISH, STRAWBERRY, PUMPKIN, CUCUMBER, SALT1,
    SWORD1, SWORD2, SWORD3, AXE1, DAGGER, MACE1, BOW, ARROW1,
    ARROW2, ARROW3, TRIDENT1, SPEAR, TRIDENT2, MACHINEGUN, PISTOL, SHOTGUN,
    GEM1, GEM2, GEM3, GEM4, GEM5, GEM6, AMULET, FOLDER,
    COIN1, SKULL, TICK, CROSS, IDOL1, IDOL2, IDOL3, IDOL4,
    SWEETCORN, BROCCOLI, BISCUIT, BERRIES, SCEPTRE, FIRE, LASERGUN1, LASERGUN2,
    HELMET1, HELMET2, HELMET3, HELMET4, __1, __2, __3, __4,
    HEART1, SLIME, COIN2, SWORD4, SWORD5, __5, DYNAMITE, MEDPACK,
    GOGGLES, HELMET5, BOMB, OILCAN, SHIRT1, SHIRT2, SPEECH, DREAM,
    HEART2, HEART3, HEART4, __6, TORCH, ICE, __7, __8,
    __9, __10, UPARROW, RING1, RING2, RING3, RING4, __11,
    TOKEN, VIAL1, VIAL2, RING5, RING6, BLOCK1, BLOCK2, BLOCK3,
    BLOCK4, BLOCK5, BLOCK6, BLOCK7, BLOCK8, BLOCK9, SHIELD1, SACK,
    BUTTON, VIAL3, VIAL4, TOOTH, BONE, HALFBLOCK1, HALFBLOCK2, HALFBLOCK3,
    HALFBLOCK4, HALFBLOCK5, HALFBLOCK6, HALFBLOCK7, HALFBLOCK8, HALFBLOCK9, SHIELD2, SHIELD3,
    FLOWER1, FLOWER2, FLOWER3, FLOWER4, STAFF1, STAFF2, STAFF3, STICK1,
    SPIKEBALL, MACE2, AXE2, __12, SWORD6, SWORD7, SWORD8, CROWN1,
    GOLD, GEM7, GEM8, GEM9, AXE3, MUSHROOM, SALT2, STICK2,
    VIAL5, VIAL6, DOT, HELMET6, SWORD9, SWORD10, SWORD11, CROWN2
        // etc... can we name all 256 built in sprites?!
};
*/

/***** Extern of Class Definition *****/
extern const mp_obj_type_t PicosystemBuffer_type;
extern const mp_obj_type_t PicosystemVoice_type;

/***** Extern of Class Methods *****/

// picosystem.cpp
extern mp_obj_t picosystem_init();
extern mp_obj_t picosystem_logo();
extern mp_obj_t picosystem_start();
extern mp_obj_t picosystem_quit();
extern mp_obj_t picosystem_flip();

// stats.cpp
extern mp_obj_t picosystem_stats();

// buffer.cpp
extern void PicosystemBuffer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PicosystemBuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PicosystemBuffer___del__(mp_obj_t self_in);
extern mp_int_t PicosystemBuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

// voice.cpp
extern void PicosystemVoice_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PicosystemVoice_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PicosystemVoice___del__(mp_obj_t self_in);
extern void PicosystemVoice_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
extern mp_obj_t PicosystemVoice_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PicosystemVoice_envelope(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PicosystemVoice_effects(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PicosystemVoice_bend(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t picosystem_audio_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

// state.cpp
extern mp_obj_t picosystem_clear();
extern mp_obj_t picosystem_pen(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_alpha(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_clip(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_blend(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_target(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_camera(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_cursor(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_spritesheet(mp_uint_t n_args, const mp_obj_t *args);

// primitives.cpp
extern mp_obj_t picosystem_pixel(mp_obj_t x_obj, mp_obj_t y_obj);
extern mp_obj_t picosystem_hline(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t c_obj);
extern mp_obj_t picosystem_vline(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t c_obj);
extern mp_obj_t picosystem_rect(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_frect(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj);
extern mp_obj_t picosystem_fcircle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj);
extern mp_obj_t picosystem_ellipse(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_fellipse(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_poly(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_fpoly(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_line(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_blit(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_sprite(mp_uint_t n_args, const mp_obj_t *args);

// text.cpp
extern mp_obj_t picosystem_text(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_measure(mp_uint_t n_args, const mp_obj_t *args);

// utility.cpp
extern mp_obj_t picosystem_rgb(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_hsv(mp_uint_t n_args, const mp_obj_t *args);

extern mp_obj_t picosystem_intersects(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_intersection(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_contains(mp_uint_t n_args, const mp_obj_t *args);

// hardware.cpp
extern mp_obj_t picosystem_pressed(mp_obj_t b_obj);
extern mp_obj_t picosystem_button(mp_obj_t b_obj);
extern mp_obj_t picosystem_battery();
extern mp_obj_t picosystem_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picosystem_backlight(mp_obj_t b_obj);
