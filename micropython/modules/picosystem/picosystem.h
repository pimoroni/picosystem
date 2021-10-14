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
    MODE_BLEND,
    MODE_MASK
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t PicosystemBuffer_type;

/***** Extern of Class Methods *****/
extern void PicosystemBuffer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PicosystemBuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PicosystemBuffer___del__(mp_obj_t self_in);
extern mp_int_t PicosystemBuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

// Declare the functions we'll make available in Python
//extern mp_obj_t picosystem_init();
//extern mp_obj_t picosystem_update(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
//extern mp_obj_t picosystem_draw();

extern mp_obj_t picosystem_init();
extern mp_obj_t picosystem_tick();

// state
extern mp_obj_t picosystem_pen(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_clip(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_blend(mp_obj_t bf_obj);
extern mp_obj_t picosystem_target(mp_obj_t dt_obj);
extern mp_obj_t picosystem_camera(mp_obj_t camx_obj, mp_obj_t camy_obj);
extern mp_obj_t picosystem_spritesheet(mp_obj_t ss_obj);

// primitives
extern mp_obj_t picosystem_clear();
extern mp_obj_t picosystem_pixel(mp_obj_t x_obj, mp_obj_t y_obj);
extern mp_obj_t picosystem_hline(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t c_obj);
extern mp_obj_t picosystem_vline(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t c_obj);
extern mp_obj_t picosystem_rect(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj);
extern mp_obj_t picosystem_poly(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_frect(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_fcircle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj);
extern mp_obj_t picosystem_fpoly(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_line(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_blit(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_sprite(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_text(mp_uint_t n_args, const mp_obj_t *args);

//void        COPY(
//              color_t* source, uint32_t source_step,
//              color_t* dest, uint32_t count);
//void        BLEND(
//              color_t* source, uint32_t source_step,
//              color_t* dest, uint32_t count);
//void        MASK(
//              color_t* source, uint32_t source_step,
//              color_t* dest, uint32_t count);

// utility
//std::string str(float v, uint8_t precision = 2);
//std::string str(int32_t v);
//std::string str(uint32_t v);
extern mp_obj_t picosystem_rgb(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_hsv(mp_uint_t n_args, const mp_obj_t *args);

//uint32_t    time();
//uint32_t    time_us();
//void        sleep(uint32_t d);
//void        sleep_us(uint32_t d);

extern mp_obj_t picosystem_intersects(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_intersection(mp_uint_t n_args, const mp_obj_t *args);
extern mp_obj_t picosystem_contains(mp_uint_t n_args, const mp_obj_t *args);

//void        wrap(std::string &t, std::size_t chars);
//std::vector<std::string> split(const std::string& t, char d = '\n');
//float       fsin(float v);
//float       fcos(float v);

// hardware
extern mp_obj_t picosystem_pressed(mp_obj_t b_obj);
extern mp_obj_t picosystem_button(mp_obj_t b_obj);
extern mp_obj_t picosystem_battery();
extern mp_obj_t picosystem_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj);
extern mp_obj_t picosystem_backlight(mp_obj_t b_obj);
