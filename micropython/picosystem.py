# PicoSystem Python Stubs

from typing import overload, Optional


COPY = 0
ALPHA = 1
MASK = 2

UP = 23
DOWN = 20
LEFT = 22
RIGHT = 21
A = 18
B = 19
X = 17
Y = 16


class Buffer():
    def __init__(self, w: int, h: int):
        """Create a new Buffer."""
        pass


class Voice():
    def __init__(self, attack: int=100, decay: int=50, sustain: int=80, release: int=100, bend: int=0, bend_ms: int=0, reverb: int=0, noise: int=0, distort: int=0):
        """Create a new Voice."""
        pass

    def play(self, note: int, duration: int=500, volume: int=100):
        """Play a note."""
        pass

    def envelope(self, attack: int=-1, decay: int=-1, sustain: int=-1, release: int=-1):
        """Change envelope parameters."""
        pass

    def effects(self, reverb: int=-1, noise: int=-1, distort: int=-1):
        """Change effect parameters."""
        pass

    def bend(self, frequency: int, duration: int=100):
        """Change pitch-bend parameters."""
        pass


def init():
    """Initialize PicoSystem.

    Done automatically at boot. Don't call!

    """


def _logo():
    """Render 1-bit centered logo in pen colour."""


def start():
    """Start PicoSystem main loop.

    Make sure you have defined:

    def update(tick):
        pass

    def draw(tick):
        pass

    """


def quit():
    """Break out of PicoSystem main loop."""


def flip():
    """Flip buffer to screen."""


def stats() -> tuple[int, int, int, int, int]:
    """Get system stats.

    Returns a tuple:
        FPS
        Idle time (us)
        Tick time (us)
        Update time (us)
        Draw time (us)

    """
    return 0, 0, 0, 0, 0


def clear():
    """Clear buffer to pen colour."""


@overload
def pen():
    """Reset pen."""


@overload
def pen(rgba: int):
    """Set pen to 16-bit colour.
    
    :param rgba: 16-bit colour value: 0xARGB.

    """


@overload
def pen(r: int, g: int, b: int):
    """Set pen colour - rgb.
    
    :param r: Red (0 to 15).
    :param g: Green (0 to 15).
    :param b: Blue (0 to 15).
    
    """


@overload
def pen(r: int, g: int, b: int, a: int):
    """Set pen colour - rgba.
    
    :param r: Red (0 to 15).
    :param g: Green (0 to 15).
    :param b: Blue (0 to 15).
    :param a: Alpha (0 to 15).
    
    """


@overload
def alpha() -> None:
    """Reset global alpha

    Resets to full alpha: 15

    """


@overload
def alpha(a: int) -> None:
    """Set global alpha.

    :param a: Alpha (0 to 15)

    """


@overload
def clip() -> None:
    """Clear clip rectangle."""


@overload
def clip(x: int, y: int, w: int, h: int) -> None:
    """Set clip rectangle.

    Call with no arguments to reset.

    """


@overload
def blend() -> None:
    """Clear blend mode."""


@overload
def blend(mode: int) -> None:
    """Set blend mode.

    One of: ALPHA, COPY, MASK

    Call with no arguments to reset.

    """


@overload
def target() -> None:
    """Reset draw target."""


@overload
def target(buffer: Buffer) -> None:
    """Set draw target."""


@overload
def camera() -> None:
    """Reset camera."""


@overload
def camera(x: int, y: int) -> None:
    """Set camera to x,y."""


@overload
def cursor() -> None:
    """Reset text cursor."""


@overload
def cursor(x: int, y: int) -> None:
    """Set text cursor."""


@overload
def spritesheet() -> None:
    """Reset spritesheet to default."""


@overload
def spritesheet(spritesheet: Buffer) -> None:
    """Set spritesheet."""


def pixel(x: int, y: int) -> None:
    """Set a single pixel."""


def line(x: int, y: int, x2: int, y2: int) -> None:
    """Draw an arbitrary line.

    :param x: Start x-position.
    :param y: Start y-position.
    :param x2: End x-position.
    :param y2: End x-position.

    """


def hline(x: int, y: int, length) -> None:
    """Draw a fast horizontal line.

    Draws a fast horizontal line, starting from
    point x,y and running for length pixels.

    """


def vline(x: int, y: int, length) -> None:
    """Draw a fast vertical line.

    Draws a fast vertical line, starting form
    point x,y and running for length pixels.

    """


def rect(x: int, y: int, w, h) -> None:
    """Draw a rectangle."""


def frect(x: int, y: int, w, h) -> None:
    """Draw a filled rectangle."""


def circle(x: int, y: int, r: int) -> None:
    """Draw a circle."""


def fcircle(x: int, y: int, r: int) -> None:
    """Draw a filled circle."""


def ellipse(x: int, y: int, rx: int, ry: int) -> None:
    """Draw an ellipse."""


def fellipse(x: int, y: int, rx: int, ry: int) -> None:
    """Draw a filled ellipse."""


@overload
def poly(points: list[tuple[int, int]]) -> None:
    """Draw a polygon."""


@overload
def poly(*args: tuple[int, int]) -> None:
    """Draw a polygon."""


@overload
def fpoly(points: list[tuple[int, int]]) -> None:
    """Draw a filled polygon."""


@overload
def fpoly(*args: tuple[int, int]) -> None:
    """Draw a filled polygon."""


@overload
def blit(buffer: Buffer, x: int, y: int, w: int, h: int, dx: int, dy: int) -> None:
    """Blit a buffer to the screen: 1:1.

    :param x: Source x-position.
    :param y: Source y-position.
    :param w: Source width.
    :param h: Source height.
    :param dx: Destination x-position.
    :param dy: Destination y-position.

    """


@overload
def blit(buffer: Buffer, x: int, y: int, w: int, h: int, dx: int, dy: int, dw: int, dh: int) -> None:
    """Blit a buffer to the screen: stretched.

    :param x: Source x-position.
    :param y: Source y-position.
    :param w: Source width.
    :param h: Source height.
    :param dx: Destination x-position.
    :param dy: Destination y-position.
    :param dx: Destination width.
    :param dy: Destination height.

    """


@overload
def sprite(index: int, x: int, y: int) -> None:
    """Draw a sprite at x,y.

    :param index: sprite index (sprites numbered 0 to 255)
    :param x: x-position.
    :param y: y-position.

    """


@overload
def sprite(index: int, x: int, y: int, cx: int, cy: int) -> None:
    """Draw a group of sprites at x,y.

    Provide a destination width and height

    :param index: sprite index (sprites numbered 0 to 255)
    :param x: x-position.
    :param y: y-position.
    :param cx: count of sprites along x axis.
    :param cy: count of sprites along y axis.

    """


@overload
def sprite(index: int, x: int, y: int, cx: int, cy: int, dw: int, dh: int) -> None:
    """Draw a group of sprites at x,y.

    Provide a destination width and height

    :param index: sprite index (sprites numbered 0 to 255)
    :param x: x-position.
    :param y: y-position.
    :param cx: count of sprites along x axis.
    :param cy: count of sprites along y axis.
    :param dw: destination width.
    :param dw: destination height.

    """


@overload
def text(text: str) -> None:
    """Draw text.

    :param text: Text to draw.

    """


@overload
def text(text: str, wrap: int) -> None:
    """Draw text.

    :param text: Text to draw.
    :param wrap: Width to wrap text.
    
    """
    pass


@overload
def text(text: str, x: int, y: int) -> None:
    """Draw text at x, y.

    :param text: Text to draw.
    :param x: x-position.
    :param y: y-position.

    """


@overload
def text(text: str, x: int, y: int, wrap: int) -> None:
    """Draw text at x, y.

    :param text: Text to draw.
    :param x: x-position.
    :param y: y-position.
    :param wrap: Width to wrap text.

    """


@overload
def measure(text: str) -> tuple[int, int]:
    """Measure text."""
    return 0, 0


@overload
def measure(text: str, wrap: int) -> tuple[int, int]:
    """Measure text, wrapped to "wrap" pixels."""
    return 0, 0


def rgb(r: int, g: int, b: int) -> int:
    """Build RGB colour."""
    return 0xFFFF


def hsv(h: float, s: float, v: float) -> int:
    """Convert HSV to RGB.

    :param h: Hue (0.0 to 1.0)
    :param s: Saturation (0.0 to 1.0)
    :param v: Value (0.0 to 1.0)

    """
    return 0xFFFF


def intersects(x, y, w, h, cx, cy, cw, ch):
    """Check if two rectangles intersect."""
    pass


def intersection(x, y, w, h, cx, cy, cw, ch):
    """Get the intersection of two rectangles.

    Returns: tuple: x, y, w, h

    """
    pass


def contains(x, y, cx, cy, cw, ch):
    """Check if rectangle contains point."""
    pass


def pressed(button: int) -> bool:
    """Check if a button has been pressed."""
    return True


def button(button: int) -> bool:
    """Check if a button is presed."""
    return True


def battery() -> int:
    """Get battery voltage."""
    return 0


def led(r: int, g: int, b: int):
    """Set the user LED.

    :param r: Red (0 to 15).
    :param g: Green (0 to 15).
    :param b: Blue (0 to 15).

    """


def backlight(brightness):
    """Set the display backlight.

    :param brightness: Brightness (0 to 100).

    """