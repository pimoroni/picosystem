import picosystem

picosystem.init()

while True:
    picosystem.tick()
    print("UP: ", picosystem.button(picosystem.UP), end=", ")
    print("DOWN: ", picosystem.button(picosystem.DOWN), end=", ")
    print("LEFT: ", picosystem.button(picosystem.LEFT), end=", ")
    print("RIGHT: ", picosystem.button(picosystem.RIGHT))
    r = 0
    g = 0
    b = 0
    if picosystem.button(picosystem.A):
        r = 255
    if picosystem.button(picosystem.B):
        g = 255
    if picosystem.button(picosystem.X):
        b = 255
    picosystem.led(r, g, b)
    if picosystem.button(picosystem.Y):
        picosystem.backlight(0)
    else:
        picosystem.backlight(255)