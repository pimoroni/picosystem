# cmake file for Pimoroni PicoSystem
set(PICO_BOARD "pimoroni_picosystem")

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(MICROPY_SOURCE_BOARD ${MICROPY_BOARD_DIR}/board.c)

set(MICROPY_C_HEAP_SIZE 4096)
