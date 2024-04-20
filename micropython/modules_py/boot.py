# PicoSystem boot script
# Runs the PicoSystem init() and runs the launcher if A is held or escape hatch if B+Y are held.
init()

if button(A):
    import launcher  # noqa: F401

if button(B) and button(Y):
    import escape  # noqa: F401
