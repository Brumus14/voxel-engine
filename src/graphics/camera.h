#ifndef CAMERA_H
#define CAMERA_H

#include "../math/vec3.h"
#include "shader_program.h"

// create custom matrix4 implementation
struct Plane {
    struct vec3d normal;
    double distance;
};

struct Frustum {
    struct Plane top_plane;
    struct Plane bottom_plane;
    struct Plane right_plane;
    struct Plane left_plane;
    struct Plane far_plane;
    struct Plane near_plane;
};

struct camera {
    struct vec3d position;
    // TODO: Use quaternion instead potentially or maybe over the top
    struct vec3d rotation;
    double fov;
    double aspect_ratio;
    double near_plane_distance;
    double far_plane_distance;
    mat4 view_matrix; // Use custom matrix structs
    mat4 projection_matrix;
};

void camera_init(struct camera *camera, struct vec3d position,
                 struct vec3d rotation, double fov, double aspect_ratio,
                 double near_plane, double far_plane);
struct vec3d camera_get_direction(struct camera *camera);
void camera_set_position(struct camera *camera, struct vec3d position);
void camera_move(struct camera *camera, struct vec3d movement_delta);
void camera_update_matrix_uniforms(struct camera *camera);
void camera_set_aspect_ratio(struct camera *camera, double aspect_ratio);
void camera_set_rotation(struct camera *camera, struct vec3d rotation);
void camera_set_fov(struct camera *camera, double fov);
void camera_rotate(struct camera *camera, struct vec3d rotation_delta);
void camera_prepare_draw(struct camera *camera);

#endif
