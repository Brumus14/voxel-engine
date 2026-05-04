#include "player.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "../math/math_util.h"
#include "../physics/collision.h"
#include "../math/cuboid.h"
#include "../data_structures/hash_map.h"
#include "../world/chunk.h"
#include "items.h"

void player_init(struct player *player, struct vec3d position,
                 struct vec3d rotation, double sensitivity,
                 struct camera *camera) {
    player->rotation = rotation;
    player->velocity = VEC3D_ZERO;
    player->position = position;
    player->sensitivity = sensitivity;
    player->camera = camera;
    player->sprinting = false;
    player->flying = true;
    player->moving = false;
    player->on_ground = false;

    // Initially manage chunks
    player->manage_chunks = true;
}

// Rename function
// dont think framerate independent, need to add in delta time somewhere
void update_movement(struct player *player, float delta_time,
                     struct world *world) {
    struct vec3d previous_position = player->position;

    struct vec3d independent_velocity =
        vec3d_scalar_multiply(player->velocity, delta_time);

    int min_x =
        floor(player->position.x - COLLISION_BOX_X / 2 + COLLISION_EPSILON);
    int max_x =
        ceil(player->position.x + COLLISION_BOX_X / 2 - COLLISION_EPSILON) - 1;
    int min_y =
        floor(player->position.y - COLLISION_BOX_Y / 2 + COLLISION_EPSILON);
    int max_y =
        ceil(player->position.y + COLLISION_BOX_Y / 2 - COLLISION_EPSILON) - 1;
    int min_z =
        floor(player->position.z - COLLISION_BOX_Z / 2 + COLLISION_EPSILON);
    int max_z =
        ceil(player->position.z + COLLISION_BOX_Z / 2 - COLLISION_EPSILON) - 1;

    if (independent_velocity.x != 0) {
        int new_min_x = floor(player->position.x + independent_velocity.x -
                              COLLISION_BOX_X / 2 + COLLISION_EPSILON);
        int new_max_x = ceil(player->position.x + independent_velocity.x +
                             COLLISION_BOX_X / 2 - COLLISION_EPSILON) -
                        1;

        int x = independent_velocity.x > 0 ? new_max_x : new_min_x;
        bool x_move = true;

        for (int y = min_y; y <= max_y; y++) {
            for (int z = min_z; z <= max_z; z++) {
                struct vec3i block_position = {x, y, z};

                if (block_is_collidable(
                        world_get_block(world, block_position))) {
                    x_move = false;
                    break;
                }
            }

            if (!x_move) {
                break;
            }
        }

        if (x_move) {
            player->position.x += independent_velocity.x;
        } else {
            player->velocity.x = 0;
            player->position.x =
                x + 0.5 -
                (0.5 + COLLISION_BOX_X / 2) * sign(independent_velocity.x);
        }

        min_x =
            floor(player->position.x - COLLISION_BOX_X / 2 + COLLISION_EPSILON);
        max_x =
            ceil(player->position.x + COLLISION_BOX_X / 2 - COLLISION_EPSILON) -
            1;
    }

    if (independent_velocity.y != 0) {
        int new_min_y = floor(player->position.y + independent_velocity.y -
                              COLLISION_BOX_Y / 2 + COLLISION_EPSILON);
        int new_max_y = ceil(player->position.y + independent_velocity.y +
                             COLLISION_BOX_Y / 2 - COLLISION_EPSILON) -
                        1;

        int y = independent_velocity.y > 0 ? new_max_y : new_min_y;
        bool y_move = true;

        for (int x = min_x; x <= max_x; x++) {
            for (int z = min_z; z <= max_z; z++) {
                struct vec3i block_position = {x, y, z};

                if (block_is_collidable(
                        world_get_block(world, block_position))) {
                    y_move = false;
                    break;
                }
            }

            if (!y_move) {
                break;
            }
        }

        if (y_move) {
            player->position.y += independent_velocity.y;
        } else {
            player->velocity.y = 0;
            player->position.y =
                y + 0.5 -
                (0.5 + COLLISION_BOX_Y / 2) * sign(independent_velocity.y);
        }

        min_y =
            floor(player->position.y - COLLISION_BOX_Y / 2 + COLLISION_EPSILON);
        max_y =
            ceil(player->position.y + COLLISION_BOX_Y / 2 - COLLISION_EPSILON) -
            1;
    }

    if (independent_velocity.z != 0) {
        int new_min_z = floor(player->position.z + independent_velocity.z -
                              COLLISION_BOX_Z / 2 + COLLISION_EPSILON);
        int new_max_z = ceil(player->position.z + independent_velocity.z +
                             COLLISION_BOX_Z / 2 - COLLISION_EPSILON) -
                        1;

        int z = independent_velocity.z > 0 ? new_max_z : new_min_z;
        bool z_move = true;

        for (int x = min_x; x <= max_x; x++) {
            for (int y = min_y; y <= max_y; y++) {
                struct vec3i block_position = {x, y, z};

                if (block_is_collidable(
                        world_get_block(world, block_position))) {
                    z_move = false;
                    break;
                }
            }

            if (!z_move) {
                break;
            }
        }

        if (z_move) {
            player->position.z += independent_velocity.z;
        } else {
            player->velocity.z = 0;
            player->position.z =
                z + 0.5 -
                (0.5 + COLLISION_BOX_Z / 2) * sign(independent_velocity.z);
        }

        min_z =
            floor(player->position.z - COLLISION_BOX_Z / 2 + COLLISION_EPSILON);
        max_z =
            ceil(player->position.z + COLLISION_BOX_Z / 2 - COLLISION_EPSILON) -
            1;
    }

    if ((int)floor(player->position.x / CHUNK_SIZE_X) !=
            (int)floor(previous_position.x / CHUNK_SIZE_X) ||
        (int)floor(player->position.y / CHUNK_SIZE_Y) !=
            (int)floor(previous_position.y / CHUNK_SIZE_Y) ||
        (int)floor(player->position.z / CHUNK_SIZE_Z) !=
            (int)floor(previous_position.z / CHUNK_SIZE_Z)) {
        player->manage_chunks = true;
    }
}

void player_update_rotation(struct player *player, struct window *window) {
    struct vec3d rotation_delta = (struct vec3d){
        -window->mouse.position_delta.y, window->mouse.position_delta.x, 0};

    vec3d_scalar_multiply_to(rotation_delta, player->sensitivity,
                             &rotation_delta);

    vec3d_add_to(player->rotation, rotation_delta, &player->rotation);

    // Clamp up and down rotation, and keep y rotation always between 0,
    // 360
    player->rotation.x = clamp(player->rotation.x, -89.9, 89.9);
    player->rotation.y = fmod(player->rotation.y + 360, 360);
}

bool is_on_ground(struct world *world, struct vec3d position) {
    int min_x = floor(position.x - COLLISION_BOX_X / 2 + COLLISION_EPSILON);
    int max_x = ceil(position.x + COLLISION_BOX_X / 2 - COLLISION_EPSILON) - 1;
    int y = floor(position.y - COLLISION_BOX_Y / 2 - GROUND_EPSILON);
    int min_z = floor(position.z - COLLISION_BOX_Z / 2 + COLLISION_EPSILON);
    int max_z = ceil(position.z + COLLISION_BOX_Z / 2 - COLLISION_EPSILON) - 1;

    bool on_ground = false;

    for (int x = min_x; x <= max_x; x++) {
        for (int z = min_z; z <= max_z; z++) {
            struct vec3i block_position = {x, y, z};

            if (block_is_collidable(world_get_block(world, block_position))) {
                on_ground = true;
                break;
            }
        }

        if (on_ground) {
            break;
        }
    }

    return on_ground;
}

void player_update_movement(struct player *player, struct window *window,
                            struct world *world) {
    double delta_time = window_get_delta_time(window);

    bool previous_on_ground = player->on_ground;
    player->on_ground = is_on_ground(world, player->position);

    if (!previous_on_ground && player->on_ground) {
        stopwatch_start(&player->on_ground_timer);
    }

    struct vec3d input = VEC3D_ZERO;

    if (keyboard_key_down(&window->keyboard, KEYCODE_D)) {
        input.x += 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_A)) {
        input.x -= 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_SPACE)) {
        input.y += 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_LEFT_SHIFT)) {
        input.y -= 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_S)) {
        input.z += 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_W)) {
        input.z -= 1;
    }

    bool previous_moving = player->moving;
    player->moving = !vec3d_equal(input, VEC3D_ZERO);

    if (player->moving &&
        keyboard_key_just_down(&window->keyboard, KEYCODE_LEFT_CONTROL)) {
        player->sprinting = !player->sprinting;
    }

    if (!previous_moving && player->moving &&
        keyboard_key_down(&window->keyboard, KEYCODE_LEFT_CONTROL)) {
        player->sprinting = true;
    }

    if (!player->moving && player->sprinting) {
        player->sprinting = false;
    }

    // Convert relative velocity delta to global velocity delta
    struct vec3d target_velocity = VEC3D_ZERO;

    struct vec3d up = (struct vec3d){0, 1, 0};
    struct vec3d forwards =
        vec3d_scalar_multiply(rotation_to_direction(player->rotation), -1);

    // Remove forwards vertical component
    forwards.y = 0;
    vec3d_normalise(&forwards);

    struct vec3d right = vec3d_cross_product(up, forwards);

    vec3d_add_to(target_velocity, vec3d_scalar_multiply(right, input.x),
                 &target_velocity);

    vec3d_add_to(target_velocity, vec3d_scalar_multiply(forwards, input.z),
                 &target_velocity);

    vec3d_normalise(&target_velocity);

    vec3d_add_to(target_velocity, vec3d_scalar_multiply(up, input.y),
                 &target_velocity);

    float speed =
        player->flying
            ? (player->sprinting ? FLYING_SPRINTING_SPEED : FLYING_SPEED)
            : (player->sprinting ? SPRINTING_SPEED : WALKING_SPEED);
    vec3d_scalar_multiply_to(target_velocity, speed, &target_velocity);

    float acceleration =
        player->on_ground ? GROUND_ACCELERATION : AIR_ACCELERATION;

    player->velocity.x +=
        acceleration * (target_velocity.x - player->velocity.x) * delta_time;

    if (!player->flying) {
        player->velocity.y -= GRAVITY_ACCELERATION * delta_time;
    } else {
        player->velocity.y += AIR_ACCELERATION *
                              (target_velocity.y - player->velocity.y) *
                              delta_time;
    }

    player->velocity.z +=
        acceleration * (target_velocity.z - player->velocity.z) * delta_time;

    if ((player->on_ground &&
         keyboard_key_just_down(&window->keyboard, KEYCODE_SPACE)) ||
        (player->on_ground &&
         keyboard_key_down(&window->keyboard, KEYCODE_SPACE) &&
         stopwatch_elapsed(&player->on_ground_timer) >= 0.05)) {
        player->velocity.y = JUMP_VELOCITY;
    }

    update_movement(player, delta_time, world);
}

void player_update(struct player *player, struct window *window,
                   struct world *world, struct hotbar *hotbar) {
    player_update_rotation(player, window);
    player_update_movement(player, window, world);

    camera_set_rotation(window->camera, player->rotation);
    camera_set_position(window->camera,
                        vec3d_add(player->position, (struct vec3d){0, 0.6, 0}));

    double target_fov = player->sprinting ? SPRINTING_FOV : DEFAULT_FOV;
    camera_set_fov(window->camera, window->camera->fov +
                                       FOV_ACCELERATION *
                                           (target_fov - window->camera->fov) *
                                           window->delta_time);

    if (mouse_button_just_down(&window->mouse, MOUSE_BUTTON_LEFT)) {
        window_capture_cursor(window);
        player_destroy_block(player, world);
    }

    if (mouse_button_just_down(&window->mouse, MOUSE_BUTTON_RIGHT)) {
        enum block_type current_block =
            item_type_to_block_type(hotbar_get_current_item(hotbar));

        if ((int)current_block != -1) {
            if (keyboard_key_down(&window->keyboard, KEYCODE_R)) {
                player_replace_block(player, world, current_block);
            } else {
                player_place_block(player, world, current_block);
            }
        }
    }

    player_manage_chunks(player, world);
}

static inline void
player_manage_chunks_chunk(struct vec3i player_chunk_position,
                           struct world *world, struct chunk *chunk) {
    if (atomic_load(&chunk->unloaded) || chunk->type != CHUNK_TYPE_FULL) {
        return;
    }

    if (abs(player_chunk_position.x - chunk->position.x) > RENDER_DISTANCE ||
        abs(player_chunk_position.y - chunk->position.y) > RENDER_DISTANCE ||
        abs(player_chunk_position.z - chunk->position.z) > RENDER_DISTANCE) {
        world_unload_chunk(world, chunk->position);
    }
}

void player_manage_chunks(struct player *player, struct world *world) {
    if (!player->manage_chunks) {
        return;
    }

    player->manage_chunks = false;

    struct vec3i player_chunk_position = {
        floor(player->position.x / CHUNK_SIZE_X),
        floor(player->position.y / CHUNK_SIZE_Y),
        floor(player->position.z / CHUNK_SIZE_Z),
    };

    // Lots of unsafe chunk loops

    unsigned int i = 0;
    struct hash_map_node *node = NULL;

    while (hash_map_iterate(&world->chunks, &i, &node)) {
        player_manage_chunks_chunk(player_chunk_position, world, node->value);
    }

    // Move this somewhere better

    for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
        for (int y = -RENDER_DISTANCE; y <= RENDER_DISTANCE; y++) {
            for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
                // Sphere chunk loading
                // if (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) >
                //     RENDER_DISTANCE) {
                //     continue;
                // }

                world_load_chunk(world,
                                 (struct vec3i){player_chunk_position.x + x,
                                                player_chunk_position.y + y,
                                                player_chunk_position.z + z});
            }
        }
    }
}

// if pointers are null
// Shouldnt this return the block type
// Create raycast function
bool player_get_target_block(struct player *player, struct world *world,
                             struct vec3d *position_dest,
                             enum block_face *face) {
    float max_ray_length = 5; // move to variable
    struct vec3d ray_origin = player->camera->position;

    // If inside block
    if (block_is_collidable(
            world_get_block(world, vec3i_from_vec3d_floor(ray_origin)))) {
        *position_dest = ray_origin;

        if (face) {
            *face = -1; // this is bit sus
        }

        return true;
    }

    // when direction is 0
    struct vec3d direction = rotation_to_direction(player->rotation);

    if (direction.x == 0) {
        direction.x = EPSILON; // use separate epsilon
    }

    if (direction.y == 0) {
        direction.y = EPSILON;
    }

    if (direction.z == 0) {
        direction.z = EPSILON;
    }

    struct vec3i voxel_position = vec3i_from_vec3d_floor(ray_origin);

    struct vec3d delta_distance = {fabs(1 / direction.x), fabs(1 / direction.y),
                                   fabs(1 / direction.z)};

    struct vec3i step = VEC3I_ZERO;
    struct vec3d side_distance = VEC3D_ZERO;
    enum block_face current_face;
    bool hit = false;

    if (direction.x < 0) {
        step.x = -1;
        side_distance.x = (ray_origin.x - voxel_position.x) * delta_distance.x;
    } else {
        step.x = 1;
        side_distance.x =
            (voxel_position.x + 1 - ray_origin.x) * delta_distance.x;
    }

    if (direction.y < 0) {
        step.y = -1;
        side_distance.y = (ray_origin.y - voxel_position.y) * delta_distance.y;
    } else {
        step.y = 1;
        side_distance.y =
            (voxel_position.y + 1 - ray_origin.y) * delta_distance.y;
    }

    if (direction.z < 0) {
        step.z = -1;
        side_distance.z = (ray_origin.z - voxel_position.z) * delta_distance.z;
    } else {
        step.z = 1;
        side_distance.z =
            (voxel_position.z + 1 - ray_origin.z) * delta_distance.z;
    }

    while (!hit && (side_distance.x < max_ray_length ||
                    side_distance.y < max_ray_length ||
                    side_distance.z < max_ray_length)) {
        if (side_distance.x < side_distance.y &&
            side_distance.x < side_distance.z) {
            side_distance.x += delta_distance.x;
            voxel_position.x += step.x;

            current_face = step.x > 0 ? BLOCK_FACE_LEFT : BLOCK_FACE_RIGHT;
        } else if (side_distance.y < side_distance.z) {
            side_distance.y += delta_distance.y;
            voxel_position.y += step.y;

            current_face = step.y > 0 ? BLOCK_FACE_BOTTOM : BLOCK_FACE_TOP;
        } else {
            side_distance.z += delta_distance.z;
            voxel_position.z += step.z;

            current_face = step.z > 0 ? BLOCK_FACE_BACK : BLOCK_FACE_FRONT;
        }

        struct vec3d position = (struct vec3d){
            voxel_position.x, voxel_position.y, voxel_position.z};

        if (world_get_block(world, vec3i_from_vec3d_floor(position))) {
            hit = true;

            *position_dest = position;

            if (face) {
                *face = current_face;
            }

            return true;
        }
    }

    return false;
}

void player_set_target_block(struct player *player, struct world *world,
                             enum block_type type) {
    struct vec3d target_block;

    if (player_get_target_block(player, world, &target_block, NULL)) {
        world_set_block(world, type, vec3i_from_vec3d_floor(target_block));
    }
}

void player_destroy_block(struct player *player, struct world *world) {
    player_set_target_block(player, world, BLOCK_TYPE_EMPTY);
}

void player_place_block(struct player *player, struct world *world,
                        enum block_type type) {
    struct vec3d target_block;
    enum block_face target_face;

    if (!player_get_target_block(player, world, &target_block, &target_face)) {
        return;
    }

    if ((int)target_face == -1) {
        return;
    }

    struct vec3d block_position = target_block;

    switch (target_face) {
    case BLOCK_FACE_LEFT:
        block_position.x--;
        break;
    case BLOCK_FACE_RIGHT:
        block_position.x++;
        break;
    case BLOCK_FACE_BOTTOM:
        block_position.y--;
        break;
    case BLOCK_FACE_TOP:
        block_position.y++;
        break;
    case BLOCK_FACE_BACK:
        block_position.z--;
        break;
    case BLOCK_FACE_FRONT:
        block_position.z++;
        break;
    }

    // Check if new block would collide with player
    struct cuboid player_cuboid;
    cuboid_init(&player_cuboid,
                player->position.x - COLLISION_BOX_X / 2 + COLLISION_EPSILON,
                player->position.y - COLLISION_BOX_Y / 2 + COLLISION_EPSILON,
                player->position.z - COLLISION_BOX_Z / 2 + COLLISION_EPSILON,
                COLLISION_BOX_X - 2 * COLLISION_EPSILON,
                COLLISION_BOX_Y - 2 * COLLISION_EPSILON,
                COLLISION_BOX_Z - 2 * COLLISION_EPSILON);

    struct cuboid new_block_cubiod;
    cuboid_init(&new_block_cubiod, block_position.x, block_position.y,
                block_position.z, 1, 1, 1);

    if (!collision_aabb_3d(player_cuboid, new_block_cubiod)) {
        world_set_block(world, type, vec3i_from_vec3d_floor(block_position));
    }
}

void player_replace_block(struct player *player, struct world *world,
                          enum block_type type) {
    player_set_target_block(player, world, type);
}
