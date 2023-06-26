# cmake file for Pimoroni PicoSystem
set(MICROPY_BOARD PIMORONI_PICOSYSTEM)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${CMAKE_CURRENT_LIST_DIR}/manifest.py)

set(MICROPY_C_HEAP_SIZE 4096)