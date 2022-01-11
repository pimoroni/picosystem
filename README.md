# PicoSystem libraries and examples

PicoSystem is a pocket sized handheld games console, built around Raspberry Pi's
RP2040 chip (that's the little fella that's the core of a Raspberry Pi Pico).

We've taken these lucky bits of silicon to the component spa and treated them to
the full works: a big chunk of flash memory, a vibrant 240x240 screen and a nice
D-pad and buttons picked out by our most particular arcade enthusiasts. There's
also a piezo speaker for discreet retro bleeps and chirps and a rechargeable
LiPo battery so you can take your homebrew games on the bus.

- [PicoSystem libraries and examples](#picosystem-libraries-and-examples)
  - [MicroPython](#micropython)
  - [C++ API](#c-api)
    - [Getting started](#getting-started)
    - [Booting PicoSystem into DFU mode](#booting-picosystem-into-dfu-mode)
    - [Example CMakeLists.txt](#example-cmakeliststxt)
    - [Build options](#build-options)
    - [Compiler flags](#compiler-flags)
  - [General Information](#general-information)
    - [Hardware](#hardware)
    - [Graphics modes](#graphics-modes)

## MicroPython

We also provide a custom MicroPython image for PicoSystem which
provides support for this API. Visit our [MicroPython PicoSystem documentation](micropython/README.md)
for more details.

## C++ API

This is the source code and documentation for the PicoSystem C++ API. It is
intentionally designed to be lightweight and get out of your way when you're
developing games for PicoSystem.

The entire API is exposed as around 40 functions which provide access to the
hardware and help with drawing, audio, and user input.

Once you're comfortable with building your project for PicoSystem you may find
our [PicoSystem C++ API Cheatsheet](https://learn.pimoroni.com/article/picosystem-api-cheatsheet) a useful reference to keep open.

### Getting started

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/.../...)

*First things first!* You must install
[Raspberry Pi's Pico SDK](https://github.com/raspberrypi/pico-sdk) following the
instructions they provide.

Then to setup the PicoSystem API simply follow these steps:

1. Create a directory to store the PicoSystem code and navigate into it
```
cd ~
mkdir picosystem
cd picosystem
```

2. Clone this repository
```
git clone git@github.com:pimoroni/picosystem.git
```

3. Create a build folder and build the examples
```
mkdir build
cd build
cmake ..
make -j8
```

You will now have the examples built in `~/picosystem/build/examples` for example
`~/picosystem/build/examples/snake/snake.uf2`. This file can be dropped onto your
PicoSystem when in [DFU mode](#booting-picosystem-into-dfu-mode).

### Booting PicoSystem into DFU mode

Hold down the `X` action button and toggle the power. PicoSystem will boot into
DFU mode and appear as a disk on your computer.

Simply drag a `.uf2` file onto the PicoSystem disk and it will be uploaded and
launch immediately.

### Example CMakeLists.txt

If you're building your own out-of-tree project you'll need to include and import the `pico_sdk_import.cmake` file,
as per regular Pico SDK setup instructions.

```cmake
cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(../../pico_sdk_import.cmake)

project(snake C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Find the PicoSystem SDK
find_package(PICOSYSTEM REQUIRED)

# Set up your project and sources
picosystem_executable(
  my_project
  my_source_file.cpp
)

# Example build options
pixel_double(my_project)
disable_startup_logo(my_project)
```

### Build options

There are a few options you can set in your project to change the behaviour of PicoSystem.

These can be set by adding one of the following lines in your `CMakeLists.txt`:

- `pixel_double(NAME)`: set resolution  to 120x120 for a retro chunky pixel aesthetic (and significant RAM saving)
- `no_spritesheet(NAME)`: do not include the default sprite sheet (saves 32kB of RAM)
- `no_font(NAME)`: do not include the default font (saves 1kB)
- `disable_startup_logo(NAME)`: disables the PicoSystem statrtup logo animation
- `no_overclock(NAME)`: disables overclocking of the RP2040 (by default we set it to 250MHz)

In all cases `NAME` should be the name you supplied to `picosystem_executable`.

### Compiler flags

If you need them, the build options are also available as compiler flags:

- `-DPIXEL_DOUBLE`: set resolution to 120x120 for a retro chunky pixel aesthetic
(saves 84kB of RAM)
- `-DNO_SPRITESHEET`: do not include the default sprite sheet (saves 32kB of
RAM)
- `-DNO_FONT`: do not include the default font (saves 1kB)
- `-DNO_STARTUP_LOGO`: disables the PicoSystem startup logo animation
- `-DNO_OVERCLOCK`: disables overclocking of the RP2040 (by default we set it
to 250MHz)

## General Information

### Hardware

- Powered by RP2040 (Dual Arm Cortex M0+ with 264kB of SRAM)
- 16MB of QSPI flash supporting XiP
- 1.54" colour SPI IPS LCD (240x240 and 120x120 modes)
- D-pad and buttons
- 525mAh LiPo battery (6hrs+)
- Piezo buzzer/speaker
- On/off power button
- RGB LED
- Programmable and rechargeable via USB-C (cable not included)

### Graphics modes

PicoSystem supports two graphics modes.

- 240x240: the native resolution of the LCD
- 120x120: pixel doubled mode (saves RAM)

Generally pixel art will look best in the pixel doubled mode (with the added
bonus that you'll save a lot of RAM!). Projects that use vector art (think
asteroids) will benefit from the native mode.

Both modes are 16 bits per pixel with 4 bits per channel (red, green, blue,
alpha). This gives a total of 4,096 possible colours and 16 different levels of
transparency.