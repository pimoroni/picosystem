import math
import time


blip = Voice(10, 0, 80, 0, 0, 0, 0, 100)
picker = Buffer(68, 68)

sliders = [
    ["R", 15, rgb(15, 0, 0)],
    ["G", 6, rgb(0, 15, 0)],
    ["B", 9, rgb(0, 0, 15)]
]

# selected colour coordinates
sx = 32
sy = 32


def colour_from_xy(x, y):
    # convert an x, y coordinate (0..63, 0..63) into an r, g, b triplet
    r = (x // 16) + (y // 16) * 4
    g = y % 16
    b = x % 16
    return int(r), int(g), int(b)


def update(tick):
    global sx, sy

    if tick % 5 == 0:
        # every 5th tick (every 50ms) check for user input and move/clamp the
        # cursor position accordingly
        if button(UP):
            sy -= 1
            sy = max(0, sy)
            blip.play(1800, 10, 100)

        if button(DOWN):
            sy += 1
            sy = min(63, sy)
            blip.play(1800, 10, 100)

        if button(LEFT):
            sx -= 1
            sx = max(0, sx)
            blip.play(1800, 10, 100)

        if button(RIGHT):
            sx += 1
            sx = min(63, sx)
            blip.play(1800, 10, 100)

        # update our selected colour from the new cursor position
        sliders[0][1], sliders[1][1], sliders[2][1] = colour_from_xy(sx, sy)


def draw_rgb_palette(x, y):
    blit(picker, 0, 0, 68, 68, x, y)

    # calculate a brightness for the cursor that pulses over time
    cursor_pulse = int((math.sin(time.ticks_ms() / 100.0) + 1.0) * 7.5)
    pen(cursor_pulse, cursor_pulse, cursor_pulse)

    # draw cursor
    hline(sx - 5 + x + 2, sy + y + 2, 3)
    hline(sx + 2 + x + 2, sy + y + 2, 3)
    vline(sx + x + 2, sy - 5 + y + 2, 3)
    vline(sx + x + 2, sy + 2 + y + 2, 3)


def prepare_rgb_palette():
    target(picker)
    blend(COPY)

    # clear to black
    pen(0, 0, 0)
    clear()

    # draw outline
    pen(8, 8, 8)
    rect(0, 0, 68, 68)

    # draw the full palette grid of 64 x 64 pixels, this covers every single
    # colour in the picosystem 4096 colour (4 bits per channel) palette.
    for py in range(64):
        for px in range(64):
            r, g, b = colour_from_xy(px, py)
            pen(r, g, b)
            pixel(px + 2, py + 2)

    target()
    blend(ALPHA)


def draw_slider(slider, x, y):
    w = 10
    h = 68

    # draw outline rectangle
    pen(slider[2])
    rect(x, y, w, h)

    # draw proportional filled value rectangle
    sh = int(((h - 4) * slider[1]) / 15)
    frect(x + 2, y + h - sh - 2, w - 4, sh)


def draw(tick):
    # clear the screen
    pen(1, 1, 1)
    clear()

    # draw title
    pen(15, 15, 15)
    frect(0, 0, 120, 11)
    pen(1, 1, 1)
    text("Palette Explorer", 2, 2)

    # draw full palette
    draw_rgb_palette(5, 18)

    # draw r, g, b value sliders
    draw_slider(sliders[0], 80, 18)
    draw_slider(sliders[1], 92, 18)
    draw_slider(sliders[2], 104, 18)

    # draw selected colour swatch
    pen(8, 8, 8)
    rect(80, 92, 34, 23)
    col = rgb(sliders[0][1], sliders[1][1], sliders[2][1])
    pen(col)
    frect(82, 94, 30, 19)

    # draw pen() call and constant value
    pen(13, 13, 13)

    pen_call = f"pen({sliders[0][1]}, {sliders[1][1]}, {sliders[2][1]})"

    text(pen_call, 5, 92)

    text(f"col = 0x{col:04x}", 5, 107)


prepare_rgb_palette()

start()
