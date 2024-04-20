import os  # noqa: E402

orig = 'main.py'
bak = f'{orig}.bak'

try:
    os.remove(bak)
except:  # noqa: E722
    pass

try:
    os.rename(orig, bak)
except:  # noqa: E722
    pass


def update(tick):
    pass


def draw(tick):
    pen(0, 0, 0)
    clear()

    pen(15, 15, 15)
    text(
        f'''Your original program, `{orig}` (if it existed), has been renamed to `{bak}`.

You may now connect your PicoSystem to a computer and fix your original program if necessary.''',
        0, 0, 120,
    )


start()
