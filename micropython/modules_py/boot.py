# PicoSystem boot script
# Runs the PicoSystem init() and launches the launcher if A is held.
from picosystem import *

init()

if button(A):
    import launcher  # noqa: F401
