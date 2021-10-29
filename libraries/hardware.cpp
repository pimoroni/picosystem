#include <math.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/vreg.h"

#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/time.h"


#ifdef PIXEL_DOUBLE
  #include "screen_double.pio.h"
#else
  #include "screen.pio.h"
#endif

#include "picosystem.hpp"

namespace picosystem {

  PIO               screen_pio  = pio0;
  uint              screen_sm   = 0;
  uint32_t          dma_channel;
  volatile int16_t  dma_scanline = -1;

  uint32_t         _audio_pwm_wrap = 5000;
  struct repeating_timer _audio_update_timer;

  enum pin {
    RED = 14, GREEN = 13, BLUE = 15,                  // user rgb led
    CS = 5, SCK = 6, MOSI  = 7,                       // spi
    VSYNC = 8, DC = 9, LCD_RESET = 4, BACKLIGHT = 12, // screen
    AUDIO = 11,                                       // audio
    CHARGE_LED = 2, CHARGING = 24, BATTERY_LEVEL = 26 // battery / charging
  };

  void init_inputs(uint32_t pin_mask) {
    for(uint8_t i = 0; i < 32; i++) {
      uint32_t pin = 1U << i;
      if(pin & pin_mask) {
        gpio_set_function(pin, GPIO_FUNC_SIO);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
      }
    }
  }

  void init_outputs(uint32_t pin_mask) {
    for(uint8_t i = 0; i < 32; i++) {
      uint32_t pin = 1U << i;
      if(pin & pin_mask) {
        gpio_set_function(pin, GPIO_FUNC_SIO);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, 0);
      }
    }
  }

  bool pressed(uint32_t b) {
    return !(_io & (1U << b)) && (_lio & (1U << b));
  }

  bool button(uint32_t b) {
    return !(_io & (1U << b));
  }

  void _reset_to_dfu() {
    reset_usb_boot(0, 0);
  }

  float _battery_voltage() {
    // convert adc reading to voltage
    adc_select_input(0);
    float v = (float(adc_read()) / (1 << 12)) * 3.3f;
    return v * 3.0f; // correct for voltage divider on board
  }

  uint32_t time() {
    absolute_time_t t = get_absolute_time();
    return to_ms_since_boot(t);
  }

  uint32_t time_us() {
    absolute_time_t t = get_absolute_time();
    return to_us_since_boot(t);
  }

  void sleep(uint32_t d) {
    sleep_ms(d);
  }

  uint32_t battery() {
    // convert to 0..1 range for battery between 2.8v and 4.1v
    float c = (_battery_voltage() - 2.8f) / 1.3f;
    return std::max(0.0f, std::min(1.0f, c)) * 100;
  }

  void _wait_vsync() {
    while(gpio_get(VSYNC))  {}  // if in vsync already wait for it to end
    while(!gpio_get(VSYNC)) {}  // now wait for vsync to occur
  }

  bool _in_flip = false;
  bool _is_flipping() {
    return _in_flip;
  }

  // in pixel doubling mode...
  //
  // scanline data is sent via dma to the pixel doubling pio program which then
  // writes the data to the st7789 via an spi-like interface. the pio program
  // doubles pixels horizontally, but we need to double them vertically by
  // sending each scanline to the pio twice.
  //
  // to minimise the number of dma transfers we transmit the current scanline
  // and the previous scanline in every transfer. the exceptions are the first
  // and final scanlines which are sent on their own to start and complete the
  // write.
  //
  // - transfer #1: scanline 0
  // - transfer #2: scanline 0 + scanline 1
  // - transfer #3: scanline 1 + scanline 2
  // ...
  // - transfer #n - 1: scanline (n - 1) + scanline n
  // - transfer #n: scanline n

  // sets up dma transfer for current and previous scanline (except for
  // scanlines 0 and 120 which are sent on their own.)
  void transmit_scanline() {
    // start of data to transmit
    uint32_t *s = (uint32_t *)&SCREEN->data[
      ((dma_scanline - 1) < 0 ? 0 : (dma_scanline - 1)) * 120
    ];
    // number of 32-bit words to transmit
    uint16_t c = (dma_scanline == 0 || dma_scanline == 120) ? 60 : 120;

    dma_channel_transfer_from_buffer_now(dma_channel, s, c);
  }

  // once the dma transfer of the scanline is complete we move to the
  // next scanline (or quit if we're finished)
  void __isr dma_complete() {
    if(dma_channel_get_irq0_status(dma_channel)) {
      dma_channel_acknowledge_irq0(dma_channel); // clear irq flag

      #ifdef PIXEL_DOUBLE
        if(++dma_scanline > 120) {
          // all scanlines done. reset counter and exit
          dma_scanline = -1;
          _in_flip = false;
          return;
        }
        transmit_scanline();
      #else
        _in_flip = false;
      #endif
    }
  }

  void _flip() {
    if(!_is_flipping()) {
      _in_flip = true;
      #ifdef PIXEL_DOUBLE
        // start the dma transfer of scanline data
        dma_scanline = 0;
        transmit_scanline();
      #else
        // if dma transfer already in process then skip
        uint32_t c = SCREEN->w * SCREEN->h / 2;
        dma_channel_transfer_from_buffer_now(dma_channel, SCREEN->data, c);
      #endif
    }
  }

  void screen_program_init(PIO pio, uint sm) {
    #ifdef PIXEL_DOUBLE
      uint offset = pio_add_program(screen_pio, &screen_double_program);
      pio_sm_config c = screen_double_program_get_default_config(offset);
    #else
      uint offset = pio_add_program(screen_pio, &screen_program);
      pio_sm_config c = screen_program_get_default_config(offset);
    #endif

    pio_sm_set_consecutive_pindirs(pio, sm, MOSI, 2, true);

    #ifndef NO_OVERCLOCK
      // dividing the clock by two ensures we keep the spi transfer to
      // around 62.5mhz as per the st7789 datasheet when overclocking
      sm_config_set_clkdiv_int_frac(&c, 2, 1);
    #endif

    // osr shifts left, autopull off, autopull threshold 32
    sm_config_set_out_shift(&c, false, false, 32);

    // configure out, set, and sideset pins
    sm_config_set_out_pins(&c, MOSI, 1);
    sm_config_set_sideset_pins(&c, SCK);

    pio_sm_set_pins_with_mask(
      pio, sm, 0, (1u << SCK) | (1u << MOSI));

    pio_sm_set_pindirs_with_mask(
      pio, sm, (1u << SCK) | (1u << MOSI), (1u << SCK) | (1u << MOSI));

    // join fifos as only tx needed (gives 8 deep fifo instead of 4)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    pio_gpio_init(screen_pio, MOSI);
    pio_gpio_init(screen_pio, SCK);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
  }

  uint16_t _gamma_correct(uint8_t v) {
    float gamma = 2.8;
    return (uint16_t)(pow((float)(v) / 100.0f, gamma) * 65535.0f + 0.5f);
  }

  void backlight(uint8_t b) {
    pwm_set_gpio_level(BACKLIGHT, _gamma_correct(b));
  }

  void _play_note(uint32_t f, uint32_t v) {
    // adjust the clock divider to achieve this desired frequency
    #ifndef NO_OVERCLOCK
      float clock = 250000000.0f;
    #else
      float clock = 125000000.0f;
    #endif

    float pwm_divider = clock / _audio_pwm_wrap / f;
    pwm_set_clkdiv(pwm_gpio_to_slice_num(AUDIO), pwm_divider);
    pwm_set_wrap(pwm_gpio_to_slice_num(AUDIO), _audio_pwm_wrap);

    // work out usable range of volumes at this frequency. the piezo speaker
    // isn't driven in a way that can control volume easily however if we're
    // clever with the duty cycle we can ensure that the ceramic doesn't have
    // time to fully deflect - effectively reducing the volume.
    //
    // through experiment it seems that constraining the deflection period of
    // the piezo to between 0 and 1/10000th of a second gives reasonable control
    // over the volume. the relationship is non linear so we also apply a
    // correction curve which is tuned so that the result sounds reasonable.
    uint32_t max_count = (f * _audio_pwm_wrap) / 10000;

    // the change in volume isn't linear - we correct for this here
    float curve = 1.8f;
    uint32_t level = (pow((float)(v) / 100.0f, curve) * max_count);
    pwm_set_gpio_level(AUDIO, level);
  }

  void led(uint8_t r, uint8_t g, uint8_t b) {
    pwm_set_gpio_level(RED,   _gamma_correct(r));
    pwm_set_gpio_level(GREEN, _gamma_correct(g));
    pwm_set_gpio_level(BLUE,  _gamma_correct(b));
  }

  void _screen_command(uint8_t c, size_t len = 0, const char *data = NULL) {
    gpio_put(CS, 0);
    gpio_put(DC, 0); // command mode
    spi_write_blocking(spi0, &c, 1);
    if(data) {
      gpio_put(DC, 1); // data mode
      spi_write_blocking(spi0, (const uint8_t*)data, len);
    }
    gpio_put(CS, 1);
  }

  uint32_t _gpio_get() {
    return gpio_get_all();
  }

  bool _audio_update_callback(struct repeating_timer *t) {
    _update_audio();
    return true;
  }

  void _start_audio() {
    add_repeating_timer_ms(-1, _audio_update_callback, NULL, &_audio_update_timer);
  }

  void _init_hardware() {
    // configure backlight pwm and disable backlight while setting up
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(BACKLIGHT), 65535);
    pwm_init(pwm_gpio_to_slice_num(BACKLIGHT), &cfg, true);
    gpio_set_function(BACKLIGHT, GPIO_FUNC_PWM);
    backlight(0);

    #ifndef NO_OVERCLOCK
      // Apply a modest overvolt, default is 1.10v.
      // this is required for a stable 250MHz on some RP2040s
      vreg_set_voltage(VREG_VOLTAGE_1_20);
	    sleep_ms(10);
      // overclock the rp2040 to 250mhz
      set_sys_clock_khz(250000, true);
    #endif

    // configure control io pins
    init_inputs(A | B | X | Y | UP | DOWN | LEFT | RIGHT);
    init_outputs(CHARGE_LED);

    // configure adc channel used to monitor battery charge
    adc_init(); adc_gpio_init(BATTERY_LEVEL);

    // configure pwm channels for red, green, blue led channels
    pwm_set_wrap(pwm_gpio_to_slice_num(RED), 65535);
    pwm_init(pwm_gpio_to_slice_num(RED), &cfg, true);
    gpio_set_function(RED, GPIO_FUNC_PWM);

    pwm_set_wrap(pwm_gpio_to_slice_num(GREEN), 65535);
    pwm_init(pwm_gpio_to_slice_num(GREEN), &cfg, true);
    gpio_set_function(GREEN, GPIO_FUNC_PWM);

    pwm_set_wrap(pwm_gpio_to_slice_num(BLUE), 65535);
    pwm_init(pwm_gpio_to_slice_num(BLUE), &cfg, true);
    gpio_set_function(BLUE, GPIO_FUNC_PWM);

    // configure the spi interface used to initialise the screen
    spi_init(spi0, 8000000);

    // reset cycle the screen before initialising
    gpio_set_function(LCD_RESET, GPIO_FUNC_SIO);
    gpio_set_dir(LCD_RESET, GPIO_OUT);
    gpio_put(LCD_RESET, 0); sleep_ms(100); gpio_put(LCD_RESET, 1);

    // configure screen io pins
    gpio_set_function(DC, GPIO_FUNC_SIO); gpio_set_dir(DC, GPIO_OUT);
    gpio_set_function(CS, GPIO_FUNC_SIO); gpio_set_dir(CS, GPIO_OUT);
    gpio_set_function(SCK, GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);

    // setup the st7789 screen driver
    gpio_put(CS, 1);

    // initialise the screen configuring it as 12-bits per pixel in RGB order
    enum st7789 {
      SWRESET   = 0x01, TEON      = 0x35, MADCTL    = 0x36, COLMOD    = 0x3A,
      GCTRL     = 0xB7, VCOMS     = 0xBB, LCMCTRL   = 0xC0, VDVVRHEN  = 0xC2,
      VRHS      = 0xC3, VDVS      = 0xC4, FRCTRL2   = 0xC6, PWRCTRL1  = 0xD0,
      FRMCTR1   = 0xB1, FRMCTR2   = 0xB2, GMCTRP1   = 0xE0, GMCTRN1   = 0xE1,
      INVOFF    = 0x20, SLPOUT    = 0x11, DISPON    = 0x29, GAMSET    = 0x26,
      DISPOFF   = 0x28, RAMWR     = 0x2C, INVON     = 0x21, CASET     = 0x2A,
      RASET     = 0x2B, STE       = 0x44, DGMEN     = 0xBA,
    };

    _screen_command(SWRESET);
    sleep_ms(5);
    _screen_command(MADCTL,    1, "\x04");
    _screen_command(TEON,      1, "\x00");
    _screen_command(FRMCTR2,   5, "\x0C\x0C\x00\x33\x33");
    _screen_command(COLMOD,    1, "\x03");
    _screen_command(GAMSET,    1, "\x01");

    _screen_command(GCTRL,     1, "\x14");
    _screen_command(VCOMS,     1, "\x25");
    _screen_command(LCMCTRL,   1, "\x2C");
    _screen_command(VDVVRHEN,  1, "\x01");
    _screen_command(VRHS,      1, "\x12");
    _screen_command(VDVS,      1, "\x20");
    _screen_command(PWRCTRL1,  2, "\xA4\xA1");
    _screen_command(FRCTRL2,   1, "\x1E");
    _screen_command(GMCTRP1,  14, "\xD0\x04\x0D\x11\x13\x2B\x3F\x54\x4C\x18\x0D\x0B\x1F\x23");
    _screen_command(GMCTRN1,  14, "\xD0\x04\x0C\x11\x13\x2C\x3F\x44\x51\x2F\x1F\x1F\x20\x23");
    _screen_command(INVON);
    sleep_ms(115);
    _screen_command(SLPOUT);
    _screen_command(DISPON);
    _screen_command(CASET,     4, "\x00\x00\x00\xef");
    _screen_command(RASET,     4, "\x00\x00\x00\xef");
    _screen_command(RAMWR);

    // switch st7789 into data mode so that we can start transmitting frame
    // data - no need to issue any more commands
    gpio_put(CS, 0);
    gpio_put(DC, 1);

    // at this stage the screen is configured and expecting to receive
    // pixel data. each time we write a screen worth of data the
    // st7789 resets the data pointer back to the start meaning that
    // we can now just leave the screen in data writing mode and
    // reassign the spi pins to our pixel doubling pio. so long as
    // we always write the entire screen we'll never get out of sync.

    // enable vsync input pin, we use this to synchronise screen updates
    // ensuring no tearing
    gpio_init(VSYNC);
    gpio_set_dir(VSYNC, GPIO_IN);

    // setup the screen updating pio program
    screen_program_init(screen_pio, screen_sm);

    // initialise dma channel for transmitting pixel data to screen
    // via the screen updating pio program
    dma_channel = 0;
    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_bswap(&config, true);
    channel_config_set_dreq(&config, pio_get_dreq(screen_pio, screen_sm, true));
    dma_channel_configure(
      dma_channel, &config, &screen_pio->txf[screen_sm], nullptr, 0, false);
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_enabled(pio_get_dreq(screen_pio, screen_sm, true), true);

    irq_set_exclusive_handler(DMA_IRQ_0, dma_complete);
    irq_set_enabled(DMA_IRQ_0, true);

    // initialise audio pwm pin
    int audio_pwm_slice_number = pwm_gpio_to_slice_num(AUDIO);
    pwm_config audio_pwm_cfg = pwm_get_default_config();
    pwm_init(audio_pwm_slice_number, &audio_pwm_cfg, true);
    gpio_set_function(AUDIO, GPIO_FUNC_PWM);
    pwm_set_gpio_level(AUDIO, 0);
  }

}