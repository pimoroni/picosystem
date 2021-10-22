import math


# a place to store the current state of the dials
class Dial:
    def __init__(self, name, unit, min, max, value, step, x, y):
        self.name = name
        self.unit = unit
        self.min = min
        self.max = max
        self.value = value
        self.step = step
        self.x = x
        self.y = y

    def adjust_value(self, v):
        self.value += v
        self.value = min(self.max, self.value)
        self.value = max(self.min, self.value)


dials = [
    Dial("frequency", "hz",    0, 8000, 440, 50, 0, 30),   # noqa E241
    Dial(   "volume",  "%",    0,  100, 100, 10, 30, 30),  # noqa E241
    Dial(  "sustain",  "%",    0,  100,  80, 10, 60, 30),  # noqa E241
    Dial(  "distort",  "%",    0,  100,   0, 10, 90, 30),  # noqa E241
    Dial(   "attack", "ms",    0, 1000, 100, 50, 0, 60),   # noqa E241
    Dial(    "decay", "ms",    0, 1000,  50, 50, 30, 60),  # noqa E241
    Dial( "duration", "ms",    0, 1000, 500, 50, 60, 60),  # noqa E241
    Dial(  "release", "ms",    0, 1000, 250, 50, 90, 60),  # noqa E241
    Dial(   "reverb", "ms",    0, 2000,   0, 50, 0, 90),   # noqa E241
    Dial(     "bend", "hz", -100,  100,   0, 10, 30, 90),  # noqa E241
    Dial(  "bend ms", "ms",    0, 1000, 100, 50, 60, 90),  # noqa E241
    Dial(    "noise",  "%",    0,  100,   0, 10, 90, 90)   # noqa E241
]

# index of the selected diable
selected = 0

# voice for the current dial settings
v = Voice()


def audio_position():
    return 0


def audio_sample(pos):
    return 0


# return a dial by name
def get_dial(name):
    for dial in dials:
        if dial.name == name:
            return dial
    return None


# change a dial value up or down and clamp to limits
def adjust_dial_value(d, delta):
    d.adjust_value(delta)


# return the current value of the named dial
def get_dial_value(name):
    dial = get_dial(name)
    if dial:
        return dial.value
    return 0


# update and play the voice
def set_voice():
    v.envelope(
        get_dial_value("attack"),
        get_dial_value("decay"),
        get_dial_value("sustain"),
        get_dial_value("release"),
    )
    v.bend(
        get_dial_value("bend"),
        get_dial_value("bend ms")
    )
    v.effects(
        get_dial_value("reverb"),
        get_dial_value("noise"),
        get_dial_value("distort")
    )
    v.play(get_dial_value("frequency"), get_dial_value("duration"), get_dial_value("volume"))


# process user input and update the world state
def update(tick):
    global selected
    change = False

    # only check for UP/DOWN state every 10 ticks (100ms) to ensure that dial
    # values change at a sensible rate
    if tick % 10 == 0:
        if button(UP):
            adjust_dial_value(dials[selected], dials[selected].step)
            change = True

        if button(DOWN):
            adjust_dial_value(dials[selected], -dials[selected].step)
            change = True

    # if LEFT or RIGHT has been pressed then move selection to the next or
    # previous dial
    if pressed(LEFT):
        selected -= 1

    if pressed(RIGHT):
        selected += 1

    selected %= len(dials)

    # if the current sound has finished playing, or we changed a dial value
    # then restart the sound
    full_duration = get_dial_value("duration") + get_dial_value("release") + get_dial_value("reverb")
    if change or audio_position() > full_duration:
        set_voice()


def draw_dial(name):
    d = get_dial(name)
    if d is None:
        return

    x = d.x
    y = d.y

    # draw outline
    pen(0, 0, 0, 2)
    rect(x, y, 30, 30)

    # draw rivets
    pen(0, 0, 0, 2)
    fcircle(x + 3, y + 3, 1)
    fcircle(x + 30 - 4, y + 3, 1)
    fcircle(x + 3, y + 30 - 4, 1)
    fcircle(x + 30 - 4, y + 30 - 3, 1)

    # draw dial face
    pen(0, 0, 0, 4)
    fcircle(x + 15, y + 15, 10)
    pen(15, 15, 15, 12)
    fcircle(x + 15, y + 15, 8)

    # draw dot of the dial
    fv = float(d.value + abs(d.min)) / (d.max - d.min)
    dotx = -int(math.sin(3.1415927 * 0.2 + fv * 3.1415927 * 1.6) * 5)
    doty = int(math.cos(3.1415927 * 0.2 + fv * 3.1415927 * 1.6) * 5)
    pen(0, 0, 0, 4)
    fcircle(dotx + x + 15, doty + y + 15, 2)

    # if this is the active dial then highlight it
    if d == dials[selected]:
        pen(0, 15, 0)
        rect(x, y, 30, 30)
        rect(x + 1, y + 1, 28, 28)


# draw the world
def draw(tick):
    # draw top row of dials
    pen(10, 12, 4)
    frect(0, 30, 120, 30)
    draw_dial("frequency")
    draw_dial("volume")
    draw_dial("sustain")
    draw_dial("distort")

    # draw middle row of dials
    pen(4, 10, 12)
    frect(0, 60, 120, 30)
    draw_dial("attack")
    draw_dial("decay")
    draw_dial("duration")
    draw_dial("release")

    # draw bottom row of dials
    pen(12, 4, 10)
    frect(0, 90, 120, 30)
    draw_dial("reverb")
    draw_dial("bend")
    draw_dial("bend ms")
    draw_dial("noise")

    d = dials[selected]
    # show dial label and value
    x = d.x
    if x >= 60:
        x -= 90
    pen(15, 15, 15, 13)
    frect(x + 30, d.y + 30 - 20, 60, 20)
    pen(0, 0, 0, 4)
    text(d.name, x + 30 + 2, d.y + 30 - 18)
    unit_label = f"{d.value}{d.unit}"
    text(unit_label, x + 30 + 2, d.y + 30 - 10)

    # draw waveform graph
    pen(0, 0, 0, 2)
    frect(0, 0, 120, 30)
    full_duration = get_dial_value("duration") + get_dial_value("release") + get_dial_value("reverb")
    pen(15, 15, 15)
    for i in range(0, 115, 6):
        s = audio_sample(int((i * full_duration) / 115))
        frect(i + 2, 25 - int(s / 2), 5, 5)

    # draw current playback marker
    pen(0, 15, 15)
    pos = int((audio_position() * 115) / full_duration)
    frect(pos + 5, 5, 2, 25)


set_voice()
start()
