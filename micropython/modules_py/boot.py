# PicoSystem boot script
import picosystem
import builtins

# Import PicoSystem module constants to builtins,
# so they are available globally.
for k, v in picosystem.__dict__.items():
    if k != "__name__":
        setattr(builtins, k, v)


# Run PicoSystem init() and launches the launcher if A is held.
init()

if button(A):
    import launcher  # noqa: F401
