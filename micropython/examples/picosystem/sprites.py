import math
import time


weapons = [
    (TRIDENT1, "Trident"),
    (SWORD1, "Sword"),
    (LASERGUN1, "Laser Gun"),
    (MACE1, "Mace"),
    (DAGGER, "Dagger"),
    (SPEAR, "Spear"),
    (BOW, "Bow"),
    (PISTOL, "Pistol"),
    (SHOTGUN, "Shotgun")
]

target_angle = 0
current_angle = 0
selected = 0
blip = Voice(10, 10, 10, 10, 40, 2)


def weapon_angle(index):
    return 360.0 / len(weapons) * index


# process user input and update the world state
def update(tick):
    global selected, target_angle

    # if LEFT or RIGHT pressed then switch the selected weapon and calculate
    # the selected weapons angle on screen to animate towards.
    if pressed(LEFT):
        selected -= 1
        blip.play(1600, 30, 100)

    if pressed(RIGHT):
        selected += 1
        blip.play(1800, 30, 100)

    selected %= len(weapons)
    target_angle = -weapon_angle(selected)


# draw the world
def draw(tick):
    global current_angle

    # clear the background
    pen(1, 1, 1)
    clear()

    pen(10, 10, 10)
    text("Choose your weapon:", 10, 10)

    # animate towards the selected weapons angle
    current_angle += (target_angle - current_angle) / 10.0

    for i in range(len(weapons)):
        # work out angle of each weapon (9 in total) and add to current rotation angle
        item_angle = weapon_angle(i) + current_angle

        # add a bounce to the selected weapon
        bounce = 0
        if i == selected:
            bounce = math.sin(math.radians(item_angle + (time.ticks_ms() / 2.0))) * 10.0 - 10.0

        # position of weapon icon on screen
        x = math.sin(math.radians(item_angle)) * 45.0
        y = math.cos(math.radians(item_angle)) * 20.0

        # size to draw weapon (closer = larger)
        scale = ((math.cos(math.radians(item_angle)) + 1.0) * 8.0) + 8.0

        # draw the weapon sprite
        sprite(
            weapons[i][0],                                                  # sprite
            int(60 + x - (scale / 2)), int(55 + y - (scale / 2) + bounce),  # position
            1, 1,
            int(scale), int(scale)                                          # size
        )

    # centre name of weapon at bottom of screen
    label_width, _ = measure(weapons[selected][1])
    pen(11, 11, 8)
    frect(int(60 - label_width / 2 - 3), 102 - 3, label_width + 6, 13)
    pen(0, 0, 0)
    text(weapons[selected][1], int(60 - (label_width / 2)), 102)


start()
