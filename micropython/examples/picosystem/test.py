fh = 0
col = 0


def update(tick):
    global h
    global col
    print("UP: ", button(UP), end=", ")
    print("DOWN: ", button(DOWN), end=", ")
    print("LEFT: ", button(LEFT), end=", ")
    print("RIGHT: ", button(RIGHT))
    r = 0
    g = 0
    b = 0
    if button(A):
        r = 100
    if button(B):
        g = 100
    if button(X):
        b = 100

    led(r, g, b)

    if button(Y):
        backlight(50)
    else:
        backlight(100)

    col = hsv(h, 1, 1)
    h += 0.001
    if h > 1.0:
        h -= 1.0


def draw(tick):
    pen(col)
    frect(10, 10, 100, 100)
    pen(15, 15, 15)
    line(10, 10, 110, 110)
    line(110, 10, 10, 110)


start()
