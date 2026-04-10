#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include "../math/math_util.h"
#include "../physics/collision.h"
#include "../math/cuboid.h"
#include "../data_structures/hash_map.h"
#include "../world/chunk.h"
#include "../util/direction.h"

void player_init(struct player *player, struct vec3d position,
                 struct vec3d rotation, double sensitivity,
                 struct camera *camera) {
    player->position = position;
    player->rotation = rotation;
    player->speed = DEFAULT_SPEED;
    player->acceleration = player->speed * 8;
    player->velocity = VEC3D_ZERO;
    player->sensitivity = sensitivity;
    player->camera = camera;
    player->sprinting = false;
}

// Rename function
// dont think framerate independent, need to add in delta time somewhere
void update_movement(struct player *player, struct window *window,
                     struct world *world) {
    for (int i = 0; i < 6; i++) {
        int x = DIRECTIONS[i].x;
        int y = DIRECTIONS[i].y;
        int z = DIRECTIONS[i].z;

        struct vec3i block_position =
            vec3i_add(vec3i_from_vec3d_floor(player->position), DIRECTIONS[i]);

        if (world_get_block(world, block_position) == BLOCK_TYPE_EMPTY) {
            continue;
        }

        struct cuboid voxel_cuboid;
        cuboid_init(&voxel_cuboid, block_position.x, block_position.y,
                    block_position.z, 1, 1, 1);

        struct cuboid player_cuboid;
        cuboid_init(&player_cuboid, player->position.x - (COLLISION_BOX_X / 2),
                    player->position.y - (COLLISION_BOX_Y / 2),
                    player->position.z - (COLLISION_BOX_Z / 2), COLLISION_BOX_X,
                    COLLISION_BOX_Y, COLLISION_BOX_Z);

        if (collision_aabb_3d(player_cuboid, voxel_cuboid)) {
            if (x != 0) {
                player->velocity.x = 0;
                player->position.x = (block_position.x + 0.5) -
                                     x * (0.5 + 0.0001 + COLLISION_BOX_X / 2);
            }

            if (y != 0) {
                player->velocity.y = 0;
                player->position.y = (block_position.y + 0.5) -
                                     y * (0.5 + 0.0001 + COLLISION_BOX_Y / 2);
            }

            if (z != 0) {
                player->velocity.z = 0;
                player->position.z = (block_position.z + 0.5) -
                                     z * (0.5 + 0.0001 + COLLISION_BOX_Z / 2);
            }
        }
    }

    double delta_time = window_get_delta_time(window);

    vec3d_add_to(player->position,
                 vec3d_scalar_multiply(player->velocity, delta_time),
                 &player->position);
}

void player_update_rotation(struct player *player, struct window *window) {
    struct vec3d rotation_delta = (struct vec3d){
        -window->mouse.position_delta.y, window->mouse.position_delta.x, 0};

    vec3d_scalar_multiply_to(rotation_delta, player->sensitivity,
                             &rotation_delta);

    vec3d_add_to(player->rotation, rotation_delta, &player->rotation);

    // Clamp up and down rotation, and keep y rotation always between 0, 360
    player->rotation.x = clamp(player->rotation.x, -89.9, 89.9);
    player->rotation.y = fmod(player->rotation.y + 360, 360);
}

void player_update_movement(struct player *player, struct window *window,
                            struct world *world) {
    double delta_time = window_get_delta_time(window);

    struct vec3d relative_velocity_delta = VEC3D_ZERO;
    struct vec3d velocity_delta = VEC3D_ZERO;

    // Calculate the relative velocity
    if (keyboard_key_down(&window->keyboard, KEYCODE_S)) {
        relative_velocity_delta.z += 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_W)) {
        relative_velocity_delta.z -= 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_A)) {
        relative_velocity_delta.x -= 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_D)) {
        relative_velocity_delta.x += 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_SPACE)) {
        relative_velocity_delta.y += 1;
    }

    if (keyboard_key_down(&window->keyboard, KEYCODE_LEFT_SHIFT)) {
        relative_velocity_delta.y -= 1;
    }

    if (vec3d_magnitude(relative_velocity_delta) == 0) {
        player->sprinting = false;
    }

    // Convert relative velocity delta to global velocity delta
    struct vec3d up = (struct vec3d){0, 1, 0};
    struct vec3d forwards =
        vec3d_scalar_multiply(rotation_to_direction(player->rotation), -1);

    // Remove forwards vertical component
    forwards.y = 0;
    vec3d_normalise(&forwards);

    struct vec3d right = vec3d_cross_product(up, forwards);

    vec3d_add_to(velocity_delta,
                 vec3d_scalar_multiply(up, relative_velocity_delta.y),
                 &velocity_delta);

    vec3d_add_to(velocity_delta,
                 vec3d_scalar_multiply(forwards, relative_velocity_delta.z),
                 &velocity_delta);

    vec3d_add_to(velocity_delta,
                 vec3d_scalar_multiply(right, relative_velocity_delta.x),
                 &velocity_delta);

    vec3d_normalise(&velocity_delta);

    vec3d_scalar_multiply_to(velocity_delta, 0.2, &velocity_delta);

    vec3d_add_to(player->velocity, velocity_delta, &player->velocity);

    vec3d_scalar_multiply_to(player->velocity, pow(0.2, delta_time),
                             &player->velocity);

    update_movement(player, window, world);
}

void player_update(struct player *player, struct window *window,
                   struct world *world) {
    player_update_rotation(player, window);
    player_update_movement(player, window, world);
    player_manage_chunks(player, world);
}

struct player_manage_chunks_chunk_context {
    struct world *world;
    struct vec3i *player_chunk;
    int *render_distance;
};

void player_manage_chunks_chunk(void *key, void *value, void *context) {
    // TODO: Potentially quite inefficient to do this every time
    struct vec3i *chunk_position = key;
    struct chunk *chunk = value;

    struct player_manage_chunks_chunk_context *c = context;
    struct world *world = c->world;
    int *render_distance = c->render_distance;
    struct vec3i *player_chunk = c->player_chunk;

    // if (!chunk) {
    //     return;
    // }

    if (abs(player_chunk->x - chunk->position.x) > *render_distance ||
        abs(player_chunk->y - chunk->position.y) > *render_distance ||
        abs(player_chunk->z - chunk->position.z) > *render_distance) {
        world_unload_chunk(world, *chunk_position);
    }
}

void player_manage_chunks(struct player *player, struct world *world) {
    // if (!player->moved_this_frame) {
    //     return;
    // }

    int render_distance = 24; // move to a variable
    struct vec3i player_chunk = {
        floor(player->position.x / CHUNK_SIZE_X),
        floor(player->position.y / CHUNK_SIZE_Y),
        floor(player->position.z / CHUNK_SIZE_Z),
    };

    // Lots of unsafe chunk loops
    struct player_manage_chunks_chunk_context context = {
        world,
        &player_chunk,
        &render_distance,
    };

    hash_map_for_each(&world->chunks, player_manage_chunks_chunk, &context);

    // Move this somewhere better

    for (int z = -render_distance; z <= render_distance; z++) {
        for (int y = -render_distance; y <= render_distance; y++) {
            for (int x = -render_distance; x <= render_distance; x++) {
                // Sphere chunk loading
                // if (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) >
                //     render_distance) {
                //     continue;
                // }

                world_load_chunk(world, (struct vec3i){player_chunk.x + x,
                                                       player_chunk.y + y,
                                                       player_chunk.z + z});
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
    if (world_get_block(world, vec3i_from_vec3d_floor(ray_origin)) !=
        BLOCK_TYPE_EMPTY) {
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
    cuboid_init(&player_cuboid, player->position.x - (COLLISION_BOX_X / 2),
                player->position.y - (COLLISION_BOX_Y / 2),
                player->position.z - (COLLISION_BOX_Z / 2), COLLISION_BOX_X,
                COLLISION_BOX_Y, COLLISION_BOX_Z);

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
