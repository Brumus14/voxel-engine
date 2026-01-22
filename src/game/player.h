#ifndef PLAYER_H
#define PLAYER_H

#include "../math/vec3.h"
#include "../graphics/camera.h"
#include "../graphics/window.h"
#include "../world/world.h"

#define DEFAULT_SPEED 10.92
#define SPRINTING_SPEED 21.6

#define COLLISION_BOX_X 0.6
#define COLLISION_BOX_Y 1.8
#define COLLISION_BOX_Z 0.6

struct player {
    struct vec3d position;
    struct vec3d rotation;
    struct camera *camera;
    double acceleration;
    double deceleration;
    double speed;
    struct vec3d velocity;
    double sensitivity;
    bool sprinting;
};

void player_init(struct player *player, struct vec3d position,
                 struct vec3d rotation, double sensitivity,
                 struct camera *camera);
void player_handle_input(struct player *player, struct window *window,
                         struct world *world);
void player_update(struct player *player, struct window *window,
                   struct world *world);
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
