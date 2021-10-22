import math
import time


piano = Voice(20, 200, 50, 50)

G3 = 196
B3 = 247
C4 = 262
D4 = 294
E4 = 330
F4 = 349
G4 = 392

notes = [
    (0, 4),
    (C4, 2), (0, 1), (G3, 1),
    (C4, 2), (0, 1), (G3, 1),
    (C4, 1), (G3, 1), (C4, 1), (E4, 1),
    (G4, 4),
    (F4, 2), (0, 1), (D4, 1),
    (F4, 2), (0, 1), (D4, 1),
    (F4, 1), (D4, 1), (B3, 1), (D4, 1),
    (G3, 4),
    (0, 4),
    (0, 4),
]

# calculate the length of the song by adding all of the note durations
song_length = sum(d for _, d in notes)

bl = 200  # beat length (ms)
ns = 15   # note spacing
sc = 62   # stave centre
ss = 6    # stave spacing
sg = 6    # stave gap

current = 0
song_tick = 0
note_tick = 0


# convert frequency in hz to a note offset (from middle C)
def frequency_to_note(f):
    steps = int(math.log2(float(f) / 440.0) * 12.0)
    octave = int((steps / 12) * 7)
    note_map = [1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7]
    note = note_map[(12 + (steps % 12)) % 12]
    return octave + note - 3


# return the y coordinate of the note so that it can be drawn at the
# correct position on the stave
def get_note_y(i):
    f, d = notes[i]
    if f == 0:
        return 0
    ny = sc - frequency_to_note(f) * (ss / 2)
    if i == current:
        ny += math.sin((float(note_tick) / (d * bl)) * 3.1415927) * 8.0
    return int(ny)


# return the offset of the note by adding up the duration of all notes that
# come before it
def note_offset(i):
    return sum(d for _, d in notes[0:i])


# draw a note shape (full, semi, rest, etc) at position x, y in the
# current pen
def draw_note_shape(n, x, y):
    f, d = n
    if f > 0:
        if d == 1:
            fcircle(x, y, 3)
            vline(x + 3, y - 14, 14)
        elif d == 2:
            circle(x, y, 3)
            vline(x + 3, y - 14, 14)
        elif d == 4:
            ellipse(x, y, 4, 3)

        if f == C4:
            hline(x - 4, y, 9)

    else:
        y = sc
        line(x - 3, y - 7, x + 2, y - 2)
        line(x + 2, y - 2, x - 3, y + 2)
        line(x - 3, y + 2, x + 3, y + 7)


# draw the note and its shadow
def draw_note(i, x, c):
    x = int(x)
    n = notes[i]
    shadow = rgb(0, 0, 0)
    y = get_note_y(i)

    pen(shadow)
    draw_note_shape(n, x + 2, y + 2)
    pen(c)
    draw_note_shape(n, x, y)


# process user input and update the world state
def update(utick):
    global note_tick, song_tick, current
    note_tick += 25
    song_tick += 25
    f, d = notes[current]

    if note_tick > (d * bl):
        # if current note has finished playing then move onto the next one
        current += 1
        if current >= len(notes):
            current = 0
            song_tick = 0

        # reset the note tick counter and start playing the note
        note_tick = 0
        if f > 0:
            piano.play(f, d * bl)


# draw the world
def draw(utick):
    # reset the camera and clear the framebuffer
    camera()
    pen(1, 2, 3)
    clear()

    # draw title and bouncing notes
    pen(15, 15, 15)
    text("Mini Mozart Maestro", 13, 10)
    sprite(189, 0, 10 + int(math.sin(time.ticks_ms() / 100.0) * 4.0))
    sprite(189, 110, 10 + int(math.sin((time.ticks_ms() + 500) / 100.0) * 4.0))

    # draw staves
    pen(6, 6, 6)
    for i in range(5):
        hline(0, sc - i * ss - sg, 240)
        hline(0, sc + i * ss + sg, 240)

    # set the camera offset to track the music as it plays
    camera(-90 + int((song_tick * ns) / bl), 0)

    # draw bar separators
    for i in range(0, song_length, 4):
        vline(i * ns - int(ns / 2), sc - (ss * 5), (ss * 10))

    # draw notes
    for i in range(len(notes)):
        c = rgb(15, 15, 15) if i == current else rgb(11, 13, 12)
        draw_note(i, note_offset(i) * ns, c)

    # decorate with dancing flowers
    camera()
    for i in range(0, 18, 4):
        y = int(math.sin(time.ticks_ms() / 200.0 + i / 2.0) * 3.0)
        sprite(181 + (1 if y > 0 else 0), i * 8 + 2, 100 + y, 1, 1, 16, 16)  # dancing flowers


start()
