#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"

// Called from main() before rp2_flash_set_timing(), after the port has brought
// the system clock up to SYS_CLK_KHZ. The overvolt must land before the
// overclock; a stock 1.10v is not enough for 250MHz on some RP2040s.
void picosystem_board_startup(void) {
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(10);
    set_sys_clock_khz(250000, true);
}
