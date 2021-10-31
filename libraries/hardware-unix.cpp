#include <math.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <map>
#include "SDL.h"

#include "picosystem.hpp"

SDL_Window* window = nullptr;
SDL_Thread *t_event_pump = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
SDL_mutex *m_flip = nullptr;
static bool running = true;
std::chrono::time_point<std::chrono::high_resolution_clock> t_start;
std::chrono::time_point<std::chrono::high_resolution_clock> t_last_flip;

const uint32_t FPS = 50;

namespace picosystem {
  uint32_t _sdl_input = 0;

  std::map<int, int> keys = {
    // arrow keys
    {SDLK_DOWN,   button::DOWN},
    {SDLK_UP,     button::UP},
    {SDLK_LEFT,   button::LEFT},
    {SDLK_RIGHT,  button::RIGHT},

    // wasd
    {SDLK_w,       button::UP},
    {SDLK_a,       button::LEFT},
    {SDLK_s,       button::DOWN},
    {SDLK_d,       button::RIGHT},

    // action buttons
    {SDLK_z,       button::A},
    {SDLK_x,       button::B},
    {SDLK_c,       button::X},
    {SDLK_v,       button::Y},

    {SDLK_u,       button::A},
    {SDLK_i,       button::B},
    {SDLK_o,       button::X},
    {SDLK_p,       button::Y},
  };

  std::map<int, int> buttons = {
    // dpad
    {SDL_CONTROLLER_BUTTON_DPAD_DOWN,   button::DOWN},
    {SDL_CONTROLLER_BUTTON_DPAD_UP,     button::UP},
    {SDL_CONTROLLER_BUTTON_DPAD_LEFT,   button::LEFT},
    {SDL_CONTROLLER_BUTTON_DPAD_RIGHT,  button::RIGHT},

    // action buttons
    {SDL_CONTROLLER_BUTTON_A,           button::A},
    {SDL_CONTROLLER_BUTTON_B,           button::B},
    {SDL_CONTROLLER_BUTTON_X,           button::X},
    {SDL_CONTROLLER_BUTTON_Y,           button::Y},
  };

  enum pin {
    RED = 14, GREEN = 13, BLUE = 15,                  // user rgb led
    CS = 5, SCK = 6, MOSI  = 7,                       // spi
    VSYNC = 8, DC = 9, LCD_RESET = 4, BACKLIGHT = 12, // screen
    AUDIO = 11,                                       // audio
    CHARGE_LED = 2, CHARGING = 24, BATTERY_LEVEL = 26 // battery / charging
  };

  int _sdl_find_key(int key) {
    auto iter = keys.find(key);
    if (iter == keys.end()) return 0;
    else return iter->second;
  }

  int _sdl_find_button(int button) {
    auto iter = buttons.find(button);
    if (iter == buttons.end()) return 0;
    else return iter->second;
  }

  void _sdl_resize() {
    SDL_LockMutex(m_flip);
		SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING, SCREEN->w, SCREEN->h);
    SDL_UnlockMutex(m_flip);
  }

  void _sdl_handle_keyboard(int key, bool state) {
    if (int button = _sdl_find_key(key)) {
      _sdl_input &= ~(1U << button);
      _sdl_input |= state << button;
    }
  }

  void _sdl_handle_controller_button(int btn, bool state) {
    if (int button = _sdl_find_button(btn)) {
      _sdl_input &= ~(1U << button);
      _sdl_input |= state << button;
    }
  }

  void _sdl_handle_event(SDL_Event &event) {
    switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;

      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          _sdl_resize();
        }
        break;

      case SDL_KEYDOWN: // fall-though
      case SDL_KEYUP:
        _sdl_handle_keyboard(event.key.keysym.sym, event.type == SDL_KEYDOWN);
        break;

      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        _sdl_handle_controller_button(event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN);
        break;

      case SDL_CONTROLLERDEVICEADDED:
        SDL_GameControllerOpen(event.cdevice.which);
        break;

      case SDL_CONTROLLERDEVICEREMOVED:
        SDL_GameControllerClose(SDL_GameControllerFromInstanceID(event.cdevice.which));
        break;

      case SDL_RENDER_TARGETS_RESET:
        std::cout << "Targets reset" << std::endl;

      case SDL_RENDER_DEVICE_RESET:
        std::cout << "Device reset" << std::endl;
        break;

      default:
        break;
    }
  }

  static int _sdl_event_pump(void *ptr) {
	  SDL_Event event;

    while (running && SDL_WaitEvent(&event)) {
      _sdl_handle_event(event);
    }

    exit(0);
  }

  bool pressed(uint32_t b) {
    return !(_io & (1U << b)) && (_lio & (1U << b));
  }

  bool button(uint32_t b) {
    return !(_io & (1U << b));
  }

  void _reset_to_dfu() {
  }

  float _battery_voltage() {
    return 3.3f;
  }

  uint32_t time() {
    auto elapsed = std::chrono::high_resolution_clock::now() - t_start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  }

  uint32_t time_us() {
    auto elapsed = std::chrono::high_resolution_clock::now() - t_start;
    return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  }

  void sleep(uint32_t d) {
    std::this_thread::sleep_for(std::chrono::milliseconds(d));
  }

  void sleep_us(uint32_t d) {
    std::this_thread::sleep_for(std::chrono::microseconds(d));
  }

  uint32_t battery() {
    // convert to 0..1 range for battery between 2.8v and 4.1v
    float c = (_battery_voltage() - 2.8f) / 1.3f;
    return std::max(0.0f, std::min(1.0f, c)) * 100;
  }

  void _wait_vsync() {}

  bool _in_flip = false;
  bool _is_flipping() {
    return _in_flip;
  }

  void _flip() {
    SDL_LockMutex(m_flip);
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = SCREEN->w;
    dest.h = SCREEN->h;
  
		SDL_UpdateTexture(texture, nullptr, SCREEN->data, SCREEN->w * 2);

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, (const SDL_Rect *)&dest);

	  SDL_RenderPresent(renderer);
    SDL_UnlockMutex(m_flip);

    t_last_flip = std::chrono::high_resolution_clock::now();
  }

  uint16_t _gamma_correct(uint8_t v) {
    float gamma = 2.8;
    return (uint16_t)(pow((float)(v) / 100.0f, gamma) * 65535.0f + 0.5f);
  }

  void backlight(uint8_t b) {
    (void)b;
  }

  void _play_note(uint32_t f, uint32_t v) {
    (void)f;
    (void)v;
  }

  void led(uint8_t r, uint8_t g, uint8_t b) {
    (void)r;
    (void)g;
    (void)b;
  }

  uint32_t _gpio_get() {
    return _sdl_input;
  }

  void _sdl_set_mode() {
    SDL_RenderSetLogicalSize(renderer, SCREEN->w, SCREEN->h);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
  }

  void _init_hardware() {
    t_start = std::chrono::high_resolution_clock::now();
    m_flip = SDL_CreateMutex();

    std::cout << "Powered by PicoSystem SDL2 debugger runtime" << std::endl << std::endl;
  	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|SDL_INIT_AUDIO) < 0) {
      std::cerr << "could not initialize SDL2: " << SDL_GetError() << std::endl;
      return;
    }

    std::cout << "Screen "  << SCREEN->w << "x" << SCREEN->h << std::endl << std::endl;

    window = SDL_CreateWindow(
      "SDL Debug",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN->w*2, SCREEN->h*2,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr) {
      std::cerr << "could not create window: " << SDL_GetError() << std::endl;
      return;
    }
    SDL_SetWindowMinimumSize(window, SCREEN->w, SCREEN->h);

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
      std::cerr << "could not create renderer: " << SDL_GetError() << std::endl;
    }

    _sdl_set_mode();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    _sdl_resize();

    t_event_pump = SDL_CreateThread(_sdl_event_pump, "Events", nullptr);
   }

}