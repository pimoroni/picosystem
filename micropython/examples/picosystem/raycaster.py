from picosystem import *

import math

# A simple raycaster demo, heavily inspired by https://lodev.org/cgtutor/raycasting.html
# Copy to your Pico along with "gadgetoid-raycaster.16bpp" for full textured output

textured = False

try:
    # Read the 16-bits per pixel ARGB4444 texture into a buffer
    # The texture map contains 32x32 pixel textures
    WALLS = Buffer(160, 160)
    open("gadgetoid-raycaster.16bpp", "rb").readinto(WALLS)
    textured = True
except OSError:
    # If texture lookup fails, fall back to untextured, solid colour walls
    del WALLS


W = 120
H = 120
MAP_W = 16
MAP_H = 16

# Texture lookup
# These map a texture index (0-5) to a solid colour
TEXTURES = [
    rgb(0, 0, 0),
    rgb(15, 0, 0),
    rgb(0, 15, 0),
    rgb(0, 0, 15),
    rgb(15, 15, 0),
    rgb(15, 0, 15)
]

# Level data
# Each byte corresponds to a tile on the map,
# non-zero tiles are considered solid (walls)
WORLD = bytearray((
    0x01, 0x02, 0x03, 0x04, 0x03, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01,
    0x01, 0x04, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01,
    0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
))

# Player starting point
# Given as a float- the whole portion corresponds to a specific tile in the map
# The fractional portion is the player position within that tile.
player_x = 3.5
player_y = 3.5

# Player direction
# Given as a 2D vector: X and Y
# This is more efficient than storing an angle when it come to ray-casting.
player_dir_x = -1
player_dir_y = 0

# Player camera plane
# Encodes the player's field-of-view into a vector
plane_x = 0
plane_y = 0.40  # FOV == 2 * atan(plane_y/1.0)

# Player movement and rotation speed
move_speed = 1.0 / 40 * 10
rotate_speed = 1.0 / 40 * 3

# Pre-calculate the sine and cosine of the rotation speed
# this is used when we rotate the player's direction vector.
sinl_rotate_speed = math.sin(rotate_speed)
cosl_rotate_speed = math.cos(rotate_speed)

sinr_rotate_speed = math.sin(-rotate_speed)
cosr_rotate_speed = math.cos(-rotate_speed)

# Horizontal size of a ray's screen column, in pixels
# A ray size of two would make a single ray responsible for two adjacent columns of pixels
# The higher this number, the lower the horizontal resolution and the faster the algorithm.
raysize = 2

# Number of rays is equal to the screen width (in pixels) divided by the ray size (in pixels)
rays = int(W / raysize)

# Lookup table for the raycasting X-offset
ray_camera_x = [2 * x / rays - 1 for x in range(rays)]


# Function to get a specific tile from the map, with bounds checking
def get_tile_at(x, y):
    x = int(x)
    y = int(y)
    if x < MAP_W and y < MAP_H:
        return WORLD[y * MAP_W + x]
    return 1  # Solid


# @micropython.native  # Uncomment to emit native instructions and speed up code slightly
def update(tick):
    global player_x, player_y, player_dir_x, player_dir_y, plane_x, plane_y

    if button(UP):  # Move forwards
        new_x = player_x + player_dir_x * move_speed
        new_y = player_y + player_dir_y * move_speed

        # Only update the player's position if the tile being
        # moved into is *empty*
        if not get_tile_at(new_x, player_y):
            player_x = new_x
        if not get_tile_at(player_x, new_y):
            player_y = new_y

    if button(DOWN):  # Move backwards
        new_x = player_x - player_dir_x * move_speed
        new_y = player_y - player_dir_y * move_speed

        # Only update the player's position if the tile being
        # moved into is *empty*
        if not get_tile_at(new_x, player_y):
            player_x = new_x
        if not get_tile_at(player_x, new_y):
            player_y = new_y

    dir_x = player_dir_x
    dir_y = player_dir_y

    if button(LEFT):  # Turn left
        # Rotate the player's direction vector
        player_dir_x = dir_x * cosl_rotate_speed - dir_y * sinl_rotate_speed
        player_dir_y = dir_x * sinl_rotate_speed + dir_y * cosl_rotate_speed

        # Rotate the camera plane
        p_x = plane_x
        plane_x = p_x * cosl_rotate_speed - plane_y * sinl_rotate_speed
        plane_y = p_x * sinl_rotate_speed + plane_y * cosl_rotate_speed

    if button(RIGHT):  # Turn right
        # Rotate the player's direction vector
        player_dir_x = dir_x * cosr_rotate_speed - dir_y * sinr_rotate_speed
        player_dir_y = dir_x * sinr_rotate_speed + dir_y * cosr_rotate_speed

        # Rotate the camera plane
        p_x = plane_x
        plane_x = p_x * cosr_rotate_speed - plane_y * sinr_rotate_speed
        plane_y = p_x * sinr_rotate_speed + plane_y * cosr_rotate_speed


# @micropython.native  # Uncomment to emit native instructions and speed up code slightly
def draw(tick):
    pen(0, 0, 6)
    clear()
    pen(0, 6, 0)
    frect(0, 60, 120, 60)

    # Step through each ray,
    # these map to a vertical column on the screen from left to right.
    for x in range(rays):
        camera_x = ray_camera_x[x]
        ray_dir_x = player_dir_x + plane_x * camera_x
        ray_dir_y = player_dir_y + plane_y * camera_x

        map_x = int(player_x)
        map_y = int(player_y)

        delta_dist_x = 1e30 if ray_dir_x == 0 else abs(1.0 / ray_dir_x)
        delta_dist_y = 1e30 if ray_dir_y == 0 else abs(1.0 / ray_dir_y)

        side_dist_x = 0
        side_dist_y = 0
        step_x = 0
        step_y = 0
        side = 0
        texture = 0

        # Based on the ray direction, figure out how far to
        # step over the map grid in the X/Y axes each iteration
        if ray_dir_x < 0:
            step_x = -1
            side_dist_x = (player_x - map_x) * delta_dist_x
        else:
            step_x = 1
            side_dist_x = (map_x + 1.0 - player_x) * delta_dist_x

        if ray_dir_y < 0:
            step_y = -1
            side_dist_y = (player_y - map_y) * delta_dist_y
        else:
            step_y = 1
            side_dist_y = (map_y + 1.0 - player_y) * delta_dist_y

        # Step the calculated X/Y distances until a wall is "hit"
        # This is an implementation of the DDA algorithm.
        # Excellent guide to how it works here: https://www.youtube.com/watch?v=NbSee-XM7WA
        while True:
            # Determine shortest axis and walk along it
            if side_dist_x < side_dist_y:
                side_dist_x += delta_dist_x
                map_x += step_x
                side = 0
            else:
                side_dist_y += delta_dist_y
                map_y += step_y
                side = 1

            # Grab the texture at the current file
            texture = get_tile_at(map_x, map_y)

            # If it's nonzero, it's a wall
            if texture > 0:
                texture -= 1  # Subtract 1 to give us a zero-based index into the texture map
                break

        perpendicular_wall_distance = 0
        wall_x = 0

        # Figure out how far along the surface of the wall we hit
        # by calculating the total travel distance of the ray
        # along the axis opposite to our collission.
        # If we hit the Y face of the wall we get the X distance
        if side == 0:
            perpendicular_wall_distance = side_dist_x - delta_dist_x
            wall_x = player_y + perpendicular_wall_distance * ray_dir_y
        else:
            perpendicular_wall_distance = side_dist_y - delta_dist_y
            wall_x = player_x + perpendicular_wall_distance * ray_dir_x

        # We only want the fractional part of the distance above
        # this gives us a scale for the X-coordinate into the wall texture
        wall_x %= 1.0

        wall_height = int(H / perpendicular_wall_distance)

        screen_x = x * raysize
        screen_y = int(-wall_height / 2 + H / 2)

        if textured:
            texture_offset = texture * 32
            texture_x = int(wall_x * 32) + texture_offset
            texture_y = 0
            texture_src_w = 1
            texture_src_h = 32

            blit(WALLS, texture_x, texture_y, texture_src_w, texture_src_h, screen_x, screen_y, raysize, wall_height)

        else:
            pen(TEXTURES[texture])
            frect(screen_x, screen_y, raysize, wall_height)


start()
