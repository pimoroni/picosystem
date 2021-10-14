#!/bin/env python3
from PIL import Image
import numpy
import sys
import pathlib

# Run with `./filename.py source-image.jpg`
IMAGE_PATH = pathlib.Path(sys.argv[1])
OUTPUT_PATH = IMAGE_PATH.with_suffix(".16bpp")

WIDTH = 128
HEIGHT = 128


def image_to_data(image):
    """Generator function to convert a PIL image to 16-bit 565 RGB bytes."""
    # NumPy is much faster at doing this. NumPy code provided by:
    # Keith (https://www.blogger.com/profile/02555547344016007163)
    pb = numpy.array(image.convert('RGB')).astype('uint16')

    # AAAA RRRR GGGG BBBB
    color = 0xF000 | ((pb[:, :, 0] & 0xF0) << 4) | (pb[:, :, 1] & 0xF0) | (pb[:, :, 2] & 0xF0 >> 4)
    return numpy.dstack(((color >> 8) & 0xFF, color & 0xFF)).flatten().astype('uint8').tobytes()


img = Image.open(IMAGE_PATH)
data = image_to_data(img)

print(f"Converted: {len(data)} bytes, Expected: {WIDTH * HEIGHT * 2} bytes.")

with open(OUTPUT_PATH, "wb") as f:
    f.write(data)

print(f"Written to: {OUTPUT_PATH}")
