#include <math.h>

#include "picosystem.hpp"

using namespace picosystem;

// a place to define and store our weapons
struct weapon_t {
  int id;
  std::string name;
};

std::array<weapon_t, 9> weapons = {{
  {TRIDENT1, "Trident"},
  {SWORD1, "Sword"},
  {LASERGUN1, "Laser Gun"},
  {MACE, "Mace"},
  {DAGGER, "Dagger"},
  {SPEAR, "Spear"},
  {BOW, "Bow"},
  {PISTOL, "Pistol"},
  {SHOTGUN, "Shotgun"}
}};

float target_angle = 0.0f;
int selected = 0;
voice_t blip;

// initialise the world
void init() {
  blip = voice(1800, 30, 100, 10, 10, 10, 10, 40, 2);
}

// process user input and update the world state
void update(uint32_t tick) {
  // if LEFT or RIGHT pressed then switch the selected weapon and calculate
  // the selected weapons angle on screen to animate towards.
  if(pressed(LEFT)) {
    selected = selected == 0 ? weapons.size() - 1 : selected - 1;
    target_angle += 360.0f / weapons.size();
    play(blip);
  }
  if(pressed(RIGHT)) {
    selected = selected == weapons.size() - 1 ? 0 : selected + 1;
    target_angle -= 360.0f / weapons.size();
    play(blip);
  }
}

// convert degrees to radians
float deg_to_rad(float d) {
  return d * (3.1415927 / 180.0f);
}

// draw the world
void draw() {
  // clear the background
  pen(1, 1, 1);
  clear();

  pen(10, 10, 10);
  text("Choose your weapon:", 10, 10);

  // animate towards the selected weapons angle
  static float angle = 0.0f;
  angle += (target_angle - angle) / 10.0f;

  for(int i = 0; i < weapons.size(); i++) {
    // work out angle of each weapon (9 in total) and add to current rotation
    // angle
    float item_angle = (i * (360.0f / weapons.size())) + angle;

    // add a bounce to the selected weapon
    int32_t bounce = 0;
    if(i == selected) {
      bounce = sin(deg_to_rad(item_angle + (time() / 2.0f))) * 10.0f - 10.0f;
    }

    // position of weapon icon on screen
    int32_t x = sin(deg_to_rad(item_angle)) * 45.0f;
    int32_t y = cos(deg_to_rad(item_angle)) * 20.0f;

    // size to draw weapon (closer = larger)
    int32_t scale = ((cos(deg_to_rad(item_angle)) + 1.0f) * 8.0f) + 8.0f;

    // draw the weapon sprite
    sprite(
      weapons[i].id,                                        // sprite id
      60 + x - (scale / 2), 55 + y - (scale / 2) + bounce,  // position
      1, 1,
      scale, scale                                          // size
    );
  }

  // centre name of weapon at bottom of screen
  int label_width = text_width(weapons[selected].name);
  pen(11, 11, 8);
  frect(60 - label_width / 2 - 3, 102 - 3, label_width + 6, 13);
  pen(0, 0, 0);
  text(weapons[selected].name, 60 - (label_width / 2), 102);
}


