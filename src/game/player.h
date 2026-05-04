#ifndef PLAYER_H
#define PLAYER_H

#include "../math/vec3.h"
#include "../graphics/camera.h"
#include "../graphics/window.h"
#include "../world/world.h"
#include "../world/block.h"
#include "../util/stopwatch.h"
#include "../gui/hotbar.h"

// #define DEFAULT_SPEED 10.92
#define WALKING_SPEED 4
#define SPRINTING_SPEED 6
#define FLYING_SPEED 20
#define FLYING_SPRINTING_SPEED 40
#define GROUND_ACCELERATION 20
#define AIR_ACCELERATION 2.5
#define AIR_DECELERATION 20
#define GRAVITY_ACCELERATION 30
#define JUMP_VELOCITY 8.5
#define DEFAULT_FOV 90
#define SPRINTING_FOV 100
#define FOV_ACCELERATION 10

#define RENDER_DISTANCE 6

#define COLLISION_BOX_X 0.6
#define COLLISION_BOX_Y 1.8
#define COLLISION_BOX_Z 0.6

#define COLLISION_EPSILON 1e-3
#define GROUND_EPSILON 1e-3

struct player {
    struct vec3d rotation;
    struct camera *camera;
    struct vec3d velocity;
    struct vec3d position;
    double sensitivity;
    bool sprinting;
    bool flying;
    bool moving;
    bool on_ground;
    bool manage_chunks;
    struct stopwatch on_ground_timer;
};

void player_init(struct player *player, struct vec3d position,
                 struct vec3d rotation, double sensitivity,
                 struct camera *camera);
void player_handle_input(struct player *player, struct window *window,
                         struct world *world);
void player_update(struct player *player, struct window *window,
                   struct world *world, struct hotbar *hotbar);
void player_manage_chunks(struct player *player, struct world *world);
bool player_get_target_block(struct player *player, struct world *world,
                             struct vec3d *position_dest,
                             enum block_face *face);
void player_set_target_block(struct player *player, struct world *world,
                             enum block_type type);
void player_destroy_block(struct player *player, struct world *world);
void player_place_block(struct player *player, struct world *world,
                        enum block_type type);
void player_replace_block(struct player *player, struct world *world,
                          enum block_type type);

#endif
