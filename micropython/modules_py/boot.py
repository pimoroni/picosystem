# PicoSystem boot script
import picosystem
import builtins
import cppmem

# Switch C++ memory allocations to use MicroPython's heap
cppmem.set_mode(cppmem.MICROPYTHON)

# Import PicoSystem module constants to builtins,
# so they are available globally.
for k, v in picosystem.__dict__.items():
    if k != "__name__":
        setattr(builtins, k, v)


# Run PicoSystem init() and launches the launcher if A is held.
init()

if button(A):
    import launcher  # noqa: F401
