import os
import math
import time
import gc

from picosystem import *


files = [file for file in os.listdir() if file.endswith(".py") and file not in ("main.py", "launcher.py")]

if "shapes.py" not in files:
    files.append("shapes")

if "colour.py" not in files:
    files.append("colour")

filecount = len(files)

target_angle = 0
current_angle = 0
selected = 0
running = True

blip = Voice(10, 10, 10, 10, 40, 2)

ding = Voice(5, 5, 100, 500, 0, 0)
ding.effects(reverb=50)
ding.bend(500, 500)


def get_item_angle(index):
    return 360.0 / filecount * index


def update(tick):
    global selected, target_angle, running

    if pressed(LEFT):
        selected -= 1
        blip.play(1600, 30, 100)

    if pressed(RIGHT):
        selected += 1
        blip.play(1800, 30, 100)

    if pressed(A):
        ding.play(880, 30, 100)
        running = False

    selected %= filecount
    target_angle = -get_item_angle(selected)


def draw():
    global current_angle

    # clear the background
    pen(1, 1, 1)
    clear()

    pen(10, 10, 10)
    text("Run a file:", 10, 10)

    current_angle += (target_angle - current_angle) / 10.0

    for i in range(filecount):
        item_angle = get_item_angle(i) + current_angle

        # add a bounce to the selected weapon
        bounce = 0
        if i == selected:
            bounce = math.sin(math.radians(item_angle + (time.ticks_ms() / 2.0))) * 10.0 - 10.0

        x = math.sin(math.radians(item_angle)) * 45.0
        y = math.cos(math.radians(item_angle)) * 20.0

        scale = ((math.cos(math.radians(item_angle)) + 1.0) * 8.0) + 7.0

        sprite(
            ord(files[i][0]),                                                  # sprite
            int(60 + x - (scale / 2)), int(55 + y - (scale / 2) + bounce),  # position
            1, 1,
            int(scale), int(scale)                                          # size
        )

    # centre name of file at bottom of screen
    label_width = text_width(files[selected])
    pen(11, 11, 8)
    frect(int(60 - label_width / 2 - 3), 102 - 3, label_width + 6, 13)
    pen(0, 0, 0)
    text(files[selected], int(60 - (label_width / 2)), 102)


while running:
    tick()
    time.sleep(1.0 / 500)


__launch_file__ = files[selected]
del get_item_angle, selected, draw, update, blip, ding, current_angle, target_angle, running
gc.collect()
reset()
__import__(__launch_file__)
