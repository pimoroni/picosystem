#!/bin/env python3
from PIL import Image
import sys
import pathlib

# Run with `./filename.py source-image.jpg`
IMAGE_PATH = pathlib.Path(sys.argv[1])
IMAGE_NAME = IMAGE_PATH.with_suffix("")
OUTPUT_PATH = IMAGE_PATH.with_suffix(".txt")


def image_to_c_array(imported_image):
    """Generator function to convert a PIL image to a C style array containing PicoSystem compatible color_t values"""
    result_array = "const color_t " + str(IMAGE_NAME) + " [" + str(imported_image.width * imported_image.height) + "] = "  + '\n' + "{ " + '\n' + "    "  

    imported_image = imported_image.convert("RGBA")

    for i in range(imported_image.width):
        for v in range(imported_image.height):
            rgb_pixel_value = imported_image.getpixel((v,i))
            r = (((rgb_pixel_value[0] - 0) * (15 - 0)) / (255 - 0)) + 0
            g = (((rgb_pixel_value[1] - 0) * (15 - 0)) / (255 - 0)) + 0
            b = (((rgb_pixel_value[2] - 0) * (15 - 0)) / (255 - 0)) + 0
            a = (((rgb_pixel_value[3] - 0) * (15 - 0)) / (255 - 0)) + 0
            converted_color = (round(r) & 0xf) | ((round(a) & 0xf) << 4) | ((round(b) & 0xf) << 8) | ((round(g) & 0xf) << 12)

            result_array += (str(hex(converted_color)) + ", ")

        if(i < imported_image.width - 1):
            result_array += '\n    '
    
    result_array += '\n' + "};"

    return result_array

img = Image.open(IMAGE_PATH)
w, h = img.size
data = image_to_c_array(img)

print(f"Converted: {w}x{h} image {IMAGE_NAME} to c array")

with open(OUTPUT_PATH, "w") as f:
    f.write(data)

print(f"Written to: {OUTPUT_PATH}")