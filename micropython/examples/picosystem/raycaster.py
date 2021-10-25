from picosystem import *

import math

# A simple raycaster demo, heavily inspired by https://lodev.org/cgtutor/raycasting.html

textured = False

try:
    WALLS = Buffer(160, 160)
    open("gadgetoid-raycaster.16bpp", "rb").readinto(WALLS)
    textured = True
except OSError:
    del WALLS


W = 120
H = 120
MAP_W = 16
MAP_H = 16

TEXTURES = [
    rgb(0, 0, 0),
    rgb(15, 0, 0),
    rgb(0, 15, 0),
    rgb(0, 0, 15),
    rgb(15, 15, 0),
    rgb(15, 0, 15)
]

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

player_x = 3.5
player_y = 3.5
player_dir_x = -1
player_dir_y = 0
plane_x = 0
plane_y = 0.40  # FOV == 2 * atan(plane_y/1.0)

move_speed = 1.0 / 40 * 10
rotate_speed = 1.0 / 40 * 3

sinl_rotate_speed = math.sin(rotate_speed)
cosl_rotate_speed = math.cos(rotate_speed)

sinr_rotate_speed = math.sin(-rotate_speed)
cosr_rotate_speed = math.cos(-rotate_speed)

raysize = 2
rays = int(W / raysize)
ray_camera_x = [2 * x / rays - 1 for x in range(rays)]


def get_tile_at(x, y):
    x = int(x)
    y = int(y)
    if x < MAP_W and y < MAP_H:
        return WORLD[y * MAP_W + x]
    return 1  # Solid


def update(tick):
    global player_x, player_y, player_dir_x, player_dir_y, plane_x, plane_y

    if button(UP):
        new_x = player_x + player_dir_x * move_speed
        new_y = player_y + player_dir_y * move_speed

        if not get_tile_at(new_x, player_y):
            player_x = new_x
        if not get_tile_at(player_x, new_y):
            player_y = new_y

    if button(DOWN):
        new_x = player_x - player_dir_x * move_speed
        new_y = player_y - player_dir_y * move_speed

        if not get_tile_at(new_x, player_y):
            player_x = new_x
        if not get_tile_at(player_x, new_y):
            player_y = new_y

    dir_x = player_dir_x
    dir_y = player_dir_y

    if button(LEFT):
        player_dir_x = dir_x * cosl_rotate_speed - dir_y * sinl_rotate_speed
        player_dir_y = dir_x * sinl_rotate_speed + dir_y * cosl_rotate_speed
        p_x = plane_x
        plane_x = p_x * cosl_rotate_speed - plane_y * sinl_rotate_speed
        plane_y = p_x * sinl_rotate_speed + plane_y * cosl_rotate_speed

    if button(RIGHT):
        player_dir_x = dir_x * cosr_rotate_speed - dir_y * sinr_rotate_speed
        player_dir_y = dir_x * sinr_rotate_speed + dir_y * cosr_rotate_speed
        p_x = plane_x
        plane_x = p_x * cosr_rotate_speed - plane_y * sinr_rotate_speed
        plane_y = p_x * sinr_rotate_speed + plane_y * cosr_rotate_speed


def draw(tick):
    pen(0, 0, 6)
    clear()
    pen(0, 6, 0)
    frect(0, 60, 120, 60)

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

        while True:
            if side_dist_x < side_dist_y:
                side_dist_x += delta_dist_x
                map_x += step_x
                side = 0
            else:
                side_dist_y += delta_dist_y
                map_y += step_y
                side = 1

            texture = get_tile_at(map_x, map_y)

            if texture > 0:
                texture -= 1
                break

        perpendicular_wall_distance = 0
        wall_x = 0

        if side == 0:
            perpendicular_wall_distance = side_dist_x - delta_dist_x
            wall_x = player_y + perpendicular_wall_distance * ray_dir_y
        else:
            perpendicular_wall_distance = side_dist_y - delta_dist_y
            wall_x = player_x + perpendicular_wall_distance * ray_dir_x

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
