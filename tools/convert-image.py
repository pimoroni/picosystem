#!/bin/env python3
from PIL import Image
import numpy
import sys
import pathlib

# Run with `./filename.py source-image.jpg`
IMAGE_PATH = pathlib.Path(sys.argv[1])
OUTPUT_PATH = IMAGE_PATH.with_suffix(".16bpp")


def image_to_data(image):
    """Generator function to convert a PIL image to 16-bit 565 RGB bytes."""
    # NumPy is much faster at doing this. NumPy code provided by:
    # Keith (https://www.blogger.com/profile/02555547344016007163)
    pb = numpy.array(image.convert('RGBA')).astype('uint16')

    r = pb[:, :, 0] // 16
    g = pb[:, :, 1] // 16
    b = pb[:, :, 2] // 16
    a = pb[:, :, 3] // 16

    # AAAA RRRR GGGG BBBB
    color = (a << 12) | (r << 8) | (g << 4) | b
    return color.flatten().byteswap().tobytes()


img = Image.open(IMAGE_PATH)
w, h = img.size
data = image_to_data(img)

print(f"Converted: {w}x{h} {len(data)} bytes")

with open(OUTPUT_PATH, "wb") as f:
    f.write(data)

print(f"Written to: {OUTPUT_PATH}")
