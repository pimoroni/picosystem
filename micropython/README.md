# PicoSystem MicroPython <!--omit in toc-->

MicroPython is a great way for beginners to get a feel for PicoSystem.

Play with audio beeps and boops via the REPL, and get a simple game running with minimal boilerplate.

- [PicoSystem MicroPython](#picosystem-micropython)
  - [Get the latest release](#get-the-latest-release)
  - [Function Reference](#function-reference)
    - [Boilerplate](#boilerplate)
    - [Just Dabbling](#just-dabbling)
    - [State](#state)
    - [Primitives](#primitives)
    - [Input](#input)
    - [Buffers](#buffers)
    - [Advanced Text Formatting](#advanced-text-formatting)
    - [Useful Bits 'n' Bobs](#useful-bits-n-bobs)
  - [Recipes & Common Usage Patterns](#recipes--common-usage-patterns)
      - [Loading a spritesheet](#loading-a-spritesheet)
  - [Build from source](#build-from-source)

## Get the latest release

Go to the [GitHub releases page](https://github.com/pimoroni/picosystem/releases/latest) to find the latest release of PicoSystem MicroPython.

* Scroll down and download the .uf2 file. eg: "picosystem-v0.1.0-micropython-v1.17.uf2"
* Connect your PicoSystem to your computer.
* Hold X while powering on your PicoSystem to enter bootloader mode.
* Copy the .uf2 file (linked below) onto the "RPI-RP2" directory that appears.

## Function Reference

### Boilerplate

Every PicoSystem MicroPython application must have the following basic form:

```python
def update(tick):
    pass

def draw(tick):
    pass

start()
```

The `update` function is where your game logic goes. `draw` is responsible for drawing your game world to the screen. Call `start()` to signal to PicoSystem
that it should start cranking the game engine.

In your `update` function you can optionally call `quit()` to break out of the game loop.

### Just Dabbling

If you're just exploring on the REPL, call `flip()` to display your changes and update input.

Try it:

```
>>> pen(0, 0, 0)
>>> clear()
>>> pen(15, 15, 15)
>>> text("Hello World")
>>> flip()
```

If you find things not working as expected on the REPL, reset your pen, cursor, camera, clip, and alpha like so:

```
>>> pen();cursor();camera();clip();alpha()
```

### State

* `pen(number)`, `pen(r, g, b)` or `pen(r, g, b, a)` - set the drawing colour
* `pen()` - reset the drawing colour to white (15, 15, 15, 15)
* `clip(x, y, w, h` - set the clipping region (drawing outside this region is ignored)
* `clip()` - reset the clipping region *to the drawing target size* (0, 0, 120, 120 if SCREEN)
* `blend(COPY / ALPHA / MASK)` - set the blend mode
* `blend()` - reset the blend mode (`ALPHA`)
* `target(Buffer)` - set a Buffer to draw into
* `target()` - reset the draw target to `SCREEN`
* `camera(x, y)` - set the camera location
* `camera()` - reset the camera location (0, 0)
* `cursor(x, y)` - set the text cursor
* `cursor()` - reset the text cursor (0, 0)
* `spritesheet(Buffer)` - set the spritesheet
* `spritesheet()` - reset the spritesheet to the builtin default

### Primitives

* `clear()` - clear to pen
* `pixel(x, y)` - single pixel
* `rect(x, y, w, h)` - rectangle
* `frect(x, y, w, h)` - filled rectangle
* `circle(x, y, r)` - circle
* `fcircle(x, y, r)` - fcircle
* `ellipse(x, y, rx, ry)` - ellipse
* `fellipse(x, y, rx, ry)` - fellipse
* `line(x1, y1, x2, y2)` - line from xy1 to xy2
* `hline(x, y, length)` - horizontal line starting from xy, length in pixels
* `vline(x, y, length)` - vertical line starting from xy, length in pixels
* `poly((x, y), (x, y), (x, y))` - polygon (supply points as tuples)
* `fpoly((x, y), (x, y), (x, y))` - filled polygon (supply points as tuples)
* `text(message)` - message at cursor position
* `text(message, wrap)` - message at cursor position, text will be wrapped on word boundaries if it exceeds wrap width in pixels
* `text(message, x, y)` - message at xy
* `text(message, x, y, wrap)` - message at xy, text will be wrapped on word boundaries if it exceeds wrap width in pixels
* `sprite(i, x, y)` - draws sprite from current spritesheet at xy
* `sprite(i, x, y, cx, cy)` - draws a rectangle of sprites from current spritesheet at xy, cx sprites across and cy sprites down
* `sprite(i, x, y, cx, cy, dw, dh)` - draws a rectangle of sprites from current spritesheet at xy, cx sprites across and cy sprites down, scaled to a rectangle dw by dh pixels
* `sprite(i, x, y, cx, cy, dw, dh, flags)` - draws a rectangle of sprites from current spritesheet at xy, cx sprites across and cy sprites down, scaled to a rectangle dw by dh pixels, flags can be any combination of HFLIP and VFLIP
* `blit(Buffer, x, y, w, h, dx, dy)` - blit a portion of a Buffer to dx,dy
* `blit(Buffer, x, y, w, h, dx, dy, dw, dh)` - blit a portion of a Buffer to dx,dy stretching to dw,dh

### Input

* `pressed(A / B / X / Y / UP / DOWN / LEFT / RIGHT)` - `True` if the button has been pressed since the last frame.
* `button(A / B / X / Y / UP / DOWN / LEFT / RIGHT)` - `True` if the button *is* pressed

### Buffers

A Buffer contains an image or spritesheet in the screen pixel format (16bit AAAARRRRGGGGBBBB).

You can draw to a Buffer using all the regular drawing operations, or draw a Buffer to the screen with `blit()`.

* `my_buffer = Buffer(width, height)` - create a new Buffer
* `open(filename, "rb").readinto(my_buffer)` - read an image into a Buffer
* `target(my_buffer)` - set your Buffer as the draw target
* `target()` - set the screen as the draw target

When `target()` is set to a Buffer, calling `clip()` will reset the clipping region
to the size of the targeted Buffer.

### Advanced Text Formatting

PicoSystem's `text()` function has some special escape sequences:

* `\\penRGBA` - set the text colour to RGBA, eg red is: `\\penF00F`
* `\\spr000` - draw a sprite in-line with text, eg a leaf: `\\spr007`

### Useful Bits 'n' Bobs

* `rgb(r, g, b)` or `rgb(r, g, b, a)` - return a new colour (16bit number)
* `hsv(hue, sat, value)` - convert from HSV to RGB and return a colour (16bit number)
* `intersects(x, y, w, h, cx, cy, cw, ch)` - check if two rectangles intersect
* `ix, iy, iw, ih = intersection(x, y, w, h, cx, cy, cw, ch)` - return the intersection between two rectangles
* `contains(x, y, cx, cy, cw, ch)` - check if a rectangle cxywh contains point xy

## Recipes & Common Usage Patterns

#### Loading a spritesheet

You can use `open(filename, "rb").readinto(Buffer)` to load one of the supplied .16bpp spritesheet files, like so:

```python
my_sprites = Buffer(128, 128)
open("spritesheet.16bpp", "rb").readinto(my_sprites)
spritesheet(my_sprites)
```

## Build from source

These steps mirror those in the GitHub actions workflow: https://github.com/pimoroni/picosystem/blob/main/.github/workflows/micropython.yml

A customised version of MicroPython is required, since PicoSystem uses some hacks that have not yet been tidied and submitted upstream.

Clone PicoSystem (if you have not already done so):

```
git clone https://github.com/pimoroni/picosystem
```

Clone MicroPython and fetch the submodules (note you must use the `experimental/picosystem` branch for now, this is subject to change):

```
git clone https://github.com/pimoroni/micropython -b experimental/picosystem
cd micropython
git submodule update --init
cd lib/pico-sdk
git submodule update --init
```

Build mpy-cross:

```
cd ../../mpy-cross
make
```

Navigate to the RP2 port:

```
cd ../ports/rp2
```

Copy frozen modules:

```
cp ../../../picosystem/micropython/modules_py/boot.py modules/
cp ../../../picosystem/micropython/examples/picosystem/colour.py modules/
cp ../../../picosystem/micropython/examples/picosystem/shapes.py modules/
cp ../../../picosystem/micropython/examples/picosystem/text.py modules/
cp ../../../picosystem/micropython/examples/picosystem/music.py modules/
cp ../../../picosystem/micropython/examples/picosystem/audio.py modules/
cp ../../../picosystem/micropython/examples/picosystem/launcher.py modules/
```

Build:

```
make USER_C_MODULES=../../../picosystem/micropython/modules/micropython.cmake BOARD=PIMORONI_PICOSYSTEM
cp build-PIMORONI_PICOSYSTEM/firmware.uf2 /path/to/RPI-RP2/
```
