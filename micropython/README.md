# PicoSystem MicroPython

## Get the latest release

## Build from source

```
git clone https://github.com/pimoroni/micropython -b board/rp2/pimoroni_picosystem
cd micropython
git submodule update --init
cd lib/pico-sdk
git submodule update --init
cd ../../micropython/mpy_cross
make
cd ../ports/rp2
cp ../../../picosystem/micropython/modules_py/boot.py modules/
make USER_C_MODULES=../../../picosystem/micropython/modules/micropython.cmake BOARD=PIMORONI_PICOSYSTEM
cp build-PIMORONI_PICOSYSTEM/firmware.uf2 /path/to/RPI-RP2/
```