#include <math.h>
#include <algorithm>
#include <array>

#include "picosystem.hpp"

using namespace picosystem;

// a place to define and store our weapons
struct weapon_t {
  int id;
  std::string name;
};

std::array<weapon_t, 9> weapons = {{
  {TRIDENT, "Trident"},
  {SWORD_IRON, "Sword"},
  {SHOTGUN_AUTO, "Laser Gun"},
  {MORNING_STAR, "Mace"},
  {SAIS, "Dagger"},
  {SPEAR, "Spear"},
  {BOW_HUNTERS, "Bow"},
  {PISTOL, "Pistol"},
  {SHOTGUN, "Shotgun"}
}};

float target_angle = 0.0f;
int selected = 0;
voice_t blip;

// initialise the world
void init() {
  blip = voice(10, 10, 10, 10, 40, 2);
}

// process user input and update the world state
void update(uint32_t tick) {
  // if LEFT or RIGHT pressed then switch the selected weapon and calculate
  // the selected weapons angle on screen to animate towards.
  if(pressed(LEFT)) {
    selected = selected == 0 ? weapons.size() - 1 : selected - 1;
    target_angle += 360.0f / weapons.size();
    play(blip, 1800, 30, 100);
  }
  if(pressed(RIGHT)) {
    selected = selected == weapons.size() - 1 ? 0 : selected + 1;
    target_angle -= 360.0f / weapons.size();
    play(blip, 1800, 30, 100);
  }
}

// convert degrees to radians
float deg_to_rad(float d) {
  return d * (3.1415927 / 180.0f);
}

// draw the world
void draw(uint32_t tick) {
  // clear the background
  alpha();
  pen(1, 1, 1);
  clear();

  pen(10, 10, 10);
  text("Choose your weapon:", 8, 10);

  // animate towards the selected weapons angle
  static float angle = 0.0f;
  angle += (target_angle - angle) / 10.0f;

  // create a sorted list of weapon ids based on how far "back" they are
  // on the screen. this means we can draw weapons at the back first and the
  // ones at the front will correctly overlay them
  std::array<uint32_t, 9> weapon_ids = {0, 1, 2, 3, 4, 5, 6, 7, 8};

  std::sort(
    weapon_ids.begin(), weapon_ids.end(),
    [](int i1, int i2) {
      float ia1 = (i1 * (360.0f / weapons.size())) + angle;
      float ia2 = (i2 * (360.0f / weapons.size())) + angle;
      return cos(deg_to_rad(ia1)) < cos(deg_to_rad(ia2));
  });

  for(auto i : weapon_ids) {
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

    // draw the shadow
    blend(PEN);
    int32_t sw = scale + (bounce / 2);
    int32_t sh = sw / 4;
    pen(0, 0, 0);
    sprite(
      weapons[i].id,                                        // sprite id
      60 + x - (sw / 2), 50 + y + (scale / 1.2),  // position
      1, 1,
      sw, sh                                        // size
    );

    blend(ALPHA);

    // draw the weapon sprite
    int32_t a = ((cos(deg_to_rad(item_angle)) + 1.0f) * 6.0f) + 4.0f;
    a = a > 15 ? 15 : a;
    alpha(a);
    sprite(
      weapons[i].id,                                        // sprite id
      60 + x - (scale / 2), 55 + y - (scale / 2) + bounce,  // position
      1, 1,
      scale, scale                                          // size
    );
    alpha(15);

    //pen(15, 15, 15);
    //text(str(a), 60 + x - (scale / 2), 55 + y - (scale / 2) + bounce);
  }



  // centre name of weapon at bottom of screen
  int32_t lw, lh;
  measure(weapons[selected].name, lw, lh);
  pen(12, 12, 12);
  frect(60 - lw / 2 - 3, 104 - 3, lw + 6, 13);
  pen(0, 0, 0);
  text(weapons[selected].name, 60 - (lw / 2), 104);
}


