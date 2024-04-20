# PicoSystem MicroPython <!--omit in toc-->

MicroPython is a great way for beginners to get a feel for PicoSystem.

Play with audio beeps and boops via the REPL, and get a simple game running with minimal boilerplate.

To view a demo that runs most of the [examples](examples/picosystem), hold `A` while powering on your PicoSystem.

## Contents

- [Getting Started](#getting-started)
  - [Getting the Latest Release](#getting-the-latest-release)
- [How to Write Programs](#how-to-write-programs)
  - [Hello, World!](#hello-world)
  - [API Reference](#api-reference)
  - [Booting with your Program](#booting-with-your-program)
  - [Just Dabbling](#just-dabbling)
  - [Buffers](#buffers)
  - [Advanced Text Formatting](#advanced-text-formatting)
  - [Recipes & Common Usage Patterns](#recipes--common-usage-patterns)
    - [Loading a Spritesheet](#loading-a-spritesheet)
- [Build from Source](#build-from-source)

## Getting Started

The [Thonny IDE](https://thonny.org) makes editing MicroPython code easy. To get started writing code:

1. Connect your PicoSystem to your computer using a USB-C cable
2. Ensure the PicoSystem is turned on
3. Select the PicoSystem from the bottom-right corner of Thonny

   > Typically, it will show up as something like "MicroPython (Raspberry Pi Pico) • Board in FS mode @ ...". If you don't see that option, ensure the board is powered on and not simply charging.

4. [Write your first program](#hello-world) and click the play button in Thonny!

### Getting the Latest Release

Go to the [GitHub releases page](https://github.com/pimoroni/picosystem/releases/latest) to find the latest release of PicoSystem MicroPython.

1. Scroll down and download the `.uf2` file. eg: `picosystem-v0.1.0-micropython-v1.17.uf2`
2. Connect your PicoSystem to your computer.
3. Hold `X` while powering on your PicoSystem to enter bootloader mode.
4. Copy the `.uf2` file (linked below) onto the `RPI-RP2` directory that appears.

> **Note:** This process will _not_ overwrite your existing code files already stored on your PicoSystem, such as `main.py`.

## How to Write Programs

Every PicoSystem MicroPython application must have the following basic form:

```python
def update(tick):
    pass

def draw(tick):
    pass

start()
```

The `update()` function is where your game logic goes (checking buttons and updating game state). `draw()` is responsible for drawing your game world to the screen. Each of these provides a `tick` parameter that tells your program how many frames have elapsed since your program started.

Call `start()` to signal to PicoSystem that it should start cranking the game engine. In your `update()` function you can optionally call `quit()` to break out of the game loop.

> **Warning:** Do not call `init()` in your code! This is called by the PicoSystem automatically, and you will need to [rescue your PicoSystem](#uh-oh-rescuing-your-picosystem) if this is run as part of your program.

### Hello, World!

Here's a simple program to get you started:

```python
HEIGHT = 120
y = 20

def update(tick):
    global y

    if button(UP) and y > 0:
        y -= 1
    
    if button(DOWN) and y < HEIGHT - 8:
        y += 1  

def draw(tick):
    pen(15, 0, 10)
    clear()

    pen(15, 15, 15)
    text('Hello, World!', 30, y)

start()
```

Be sure to check out the [examples](examples/picosystem) for ideas on how to leverage more of the built-in PicoSystem functions. You can also refer to the [API Reference](#api-reference) below.

### API Reference

There's a lot more you can do than what's shown in [Hello, World!](#hello-world) The complete list of built-in functions and their descriptions can be found in [`picosystem.py`](picosystem.py)

### Booting with your Program

To make your program to run whenever your PicoSystem boots, simply name your code file `main.py` when saving to your PicoSystem.

#### Uh Oh! Rescuing your PicoSystem

Have you coded yourself into some trouble and are finding your PicoSystem will no longer start up or communicate with your computer? Simply hold the `B` and `Y` buttons (as in "bye!") while powering on your PicoSystem. If this doesn't seem to work, ensure you have the [latest firmware](#getting-the-latest-release) on your PicoSystem.

This will rename your current `main.py` to `main.py.bak` so that your PicoSystem will start up properly next time. To continue editing your code after reconnecting to Thonny (per the [Getting Started](#getting-started) steps), just open `main.py.bak` from your PicoSystem, fix any errors, and re-save it as `main.py`. It's also a good idea to delete `main.py.bak` when you're done.

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

### Recipes & Common Usage Patterns

#### Loading a Spritesheet

You can use `open(filename, "rb").readinto(Buffer)` to load one of the supplied .16bpp spritesheet files, like so:

```python
my_sprites = Buffer(128, 128)
open("spritesheet.16bpp", "rb").readinto(my_sprites)
spritesheet(my_sprites)
```

## Build from Source

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
cp ../../../picosystem/micropython/modules_py/* modules/
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
