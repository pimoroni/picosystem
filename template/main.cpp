#include "picosystem.hpp"

using namespace picosystem;

extern char _binary_s4m_ur4i_platformer_16bpp_start[];
buffer_t *platformer = buffer(128, 128, (void *)_binary_s4m_ur4i_platformer_16bpp_start);

void init() {
    spritesheet(platformer);
}

void update(uint32_t tick) {
}

void draw(uint32_t tick) {
    pen(0, 0, 0);
    clear();
    pen(15, 15, 15);
    text("Hello World", 14, 12);
    sprite(192, 4, 4);
    sprite(192 + 16, 4, 12);
}