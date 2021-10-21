spritesheets = [
    "s4m_ur4i-dingbads.16bpp",
    "s4m_ur4i-pirate-characters.16bpp",
    "s4m_ur4i-pirate-tilemap.16bpp",
    "s4m_ur4i-platformer.16bpp",
    "s4m_ur4i-space-shooter-backdrop.16bpp",
    "s4m_ur4i-space-shooter-ships.16bpp",
    "s4m_ur4i-top-down-shooter.16bpp"
]

buffer = Buffer(128, 128)
spritesheet(buffer)
last_update = 0
current_sheet = 0


def update(tick):
    global last_update, current_sheet
    if tick - last_update > 100:
        last_update = tick
        open(spritesheets[current_sheet], "rb").readinto(buffer)
        current_sheet += 1
        if current_sheet >= len(spritesheets):
            current_sheet = 0


def draw(tick):
    pen(0, 0, 0)
    clear()
    for x in range(int(120 / 8)):
        for y in range(int(120 / 8)):
            sprite(x + y * int(120 / 8), x * 8, y * 8)


start()
