freeze("$(PORT_DIR)/modules/")
freeze("$(BOARD_DIR)/../modules_py/")
include("$(MPY_DIR)/extmod/uasyncio")
require("onewire")
require("ds18x20")
require("dht")
require("neopixel")