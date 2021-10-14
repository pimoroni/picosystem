# PicoSystem boot script
# Imports the whole PicoSystem API and runs init()
from picosystem import *
init()

if button(A):
    import shapes
