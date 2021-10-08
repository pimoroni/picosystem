import picosystem

picosystem.init()

h = 0
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
    col = picosystem.hsv(h, 1, 0.5)
    h += 0.001
    if h > 1.0:
        h -= 1.0
       
    picosystem.pen(col)
    picosystem.frect(10, 10, 110, 110)
    picosystem.pen(15, 15, 15)
    picosystem.line(10, 10, 110, 110)
    picosystem.line(110, 10, 10, 110)
    