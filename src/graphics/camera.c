#include "camera.h"

#include "cglm/mat4.h"
#include "glad/glad.h"
#include "../util/gl.h"
#include "../math/math_util.h"
#include <math.h>
#include <stdio.h>

void plane_normalise(float *plane) {
    float magnitude =
        sqrt(pow(plane[0], 2) + pow(plane[1], 2) + pow(plane[2], 2));

    plane[0] /= magnitude;
    plane[1] /= magnitude;
    plane[2] /= magnitude;
    plane[3] /= magnitude;
}

float plane_distance(float *plane, struct vec3d point) {
    return plane[0] * point.x + plane[1] * point.y + plane[2] * point.z +
           plane[3];
}

void generate_projection_matrix(struct camera *camera) {
    if (!camera) {
        fprintf(stderr, "generate_perspective_matrix: struct camera is null\n");
        return;
    }

    glm_perspective(glm_rad(camera->fov), camera->aspect_ratio,
                    camera->near_plane_distance, camera->far_plane_distance,
                    camera->projection_matrix);

    camera->projection_matrix_stale = false;
}

void generate_view_matrix(struct camera *camera) {
    if (!camera) {
        fprintf(stderr, "generate_view_matrix: struct camera is null\n");
        return;
    }

    vec3 glm_direction;
    vec3d_to_glm(camera_get_direction(camera), &glm_direction);

    vec3 glm_position;
    vec3d_to_glm(camera->position, &glm_position);

    glm_look(glm_position, glm_direction, (vec3){0.0, 1.0, 0.0},
             camera->view_matrix);

    camera->view_matrix_stale = false;
}

void camera_init(struct camera *camera, struct vec3d position,
                 struct vec3d rotation, double fov, double aspect_ratio,
                 double near_plane_distance, double far_plane_distance) {
    if (!camera) {
        fprintf(stderr, "camera_init: struct camera is null\n");
        return;
    }

    vec3 glm_position;
    vec3d_to_glm(position, &glm_position);

    camera->position = position;
    camera->rotation = rotation;
    camera->fov = fov;
    camera->aspect_ratio = aspect_ratio;
    camera->near_plane_distance = near_plane_distance;
    camera->far_plane_distance = far_plane_distance;
    glm_mat4_zero(camera->view_matrix);
    glm_mat4_zero(camera->projection_matrix);
    camera->view_matrix_stale = true;
    camera->projection_matrix_stale = true;
    glm_mat4_zero(camera->view_projection_matrix);
}

struct vec3d camera_get_direction(struct camera *camera) {
    return rotation_to_direction(camera->rotation);
}

void camera_set_position(struct camera *camera, struct vec3d position) {
    camera->position = position;
    camera->view_matrix_stale = true;
}

// Relative to rotation
void camera_move(struct camera *camera, struct vec3d movement_delta) {
    if (!camera) {
        fprintf(stderr, "camera_translate: struct camera is null\n");
        return;
    }

    struct vec3d position_delta;
    vec3d_init(&position_delta, 0.0, 0.0, 0.0);

    struct vec3d forwards = camera_get_direction(camera);
    forwards.y = 0.0;
    vec3d_normalise(&forwards);

    position_delta = vec3d_add(
        position_delta, vec3d_scalar_multiply(forwards, movement_delta.z));

    struct vec3d up;
    vec3d_init(&up, 0.0, 1.0, 0.0);

    position_delta =
        vec3d_add(position_delta,
                  vec3d_scalar_multiply(vec3d_normalised(vec3d_cross_product(
                                            camera_get_direction(camera), up)),
                                        movement_delta.x));

    position_delta.y += movement_delta.y;

    camera->position = vec3d_add(camera->position, position_delta);
    camera->view_matrix_stale = true;
}

void camera_update_matrix_uniforms(struct camera *camera) {
    if (!camera) {
        fprintf(stderr,
                "camera_update_matrix_uniforms: struct camera is null\n");
        return;
    }

    // abstract setting uniform value
    // this is bad stuff
    GLint shader_program_id;
    GL_CALL(glGetIntegerv(GL_CURRENT_PROGRAM, &shader_program_id));

    GLint view_projection_loc = GL_CALL_R(
        glGetUniformLocation(shader_program_id, "view_projection_matrix"),
        GLint);

    GL_CALL(glUniformMatrix4fv(
        view_projection_loc, 1, GL_FALSE,
        (float *)
            camera->view_projection_matrix)); // reduce casting for performance

    // should this be moved somewhere else?
    mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
    GLint model_loc =
        GL_CALL(glGetUniformLocation(shader_program_id, "model_matrix"));
    GL_CALL(glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model_matrix));

    GLint camera_position_loc = GL_CALL_R(
        glGetUniformLocation(shader_program_id, "camera_position"), GLint);

    GL_CALL(glUniform3f(camera_position_loc, camera->position.x,
                        camera->position.y, camera->position.z));
}

void camera_set_aspect_ratio(struct camera *camera, double aspect_ratio) {
    if (!camera) {
        fprintf(stderr, "camera_set_aspect_ratio: struct camera is null\n");
        return;
    }

    camera->aspect_ratio = aspect_ratio;
    camera->projection_matrix_stale = true;
}

void camera_set_rotation(struct camera *camera, struct vec3d rotation) {
    if (!camera) {
        fprintf(stderr, "camera_set_rotation: struct camera is null\n");
        return;
    }

    camera->rotation = rotation;
    camera->view_matrix_stale = true;
}

void camera_set_fov(struct camera *camera, double fov) {
    if (!camera) {
        fprintf(stderr, "camera_set_rotation: struct camera is null\n");
        return;
    }

    camera->fov = fov;
    camera->projection_matrix_stale = true;
}

void camera_rotate(struct camera *camera, struct vec3d rotation_delta) {
    if (!camera) {
        fprintf(stderr, "camera_rotate: struct camera is null\n");
        return;
    }

    struct vec3d new_rotation = vec3d_add(camera->rotation, rotation_delta);
    camera_set_rotation(camera, new_rotation);
}

void camera_update(struct camera *camera) {
    bool view_projection_matrix_stale = false;

    if (camera->view_matrix_stale) {
        generate_view_matrix(camera);
        view_projection_matrix_stale = true;
    }

    if (camera->projection_matrix_stale) {
        generate_projection_matrix(camera);
        view_projection_matrix_stale = true;
    }

    if (view_projection_matrix_stale) {
        glm_mat4_mul(camera->projection_matrix, camera->view_matrix,
                     camera->view_projection_matrix);
        camera_update_frustum(camera);
        camera_update_matrix_uniforms(camera);
    }
}

void camera_update_frustum(struct camera *camera) {
    struct vec3d forward = camera_get_direction(camera);
    struct vec3d right =
        vec3d_normalised(vec3d_cross_product(forward, (struct vec3d){0, 1, 0}));
    struct vec3d up = vec3d_normalised(vec3d_cross_product(right, forward));

    float half_v_side =
        tan(camera->fov / 2 * (M_PI / 180)) * camera->far_plane_distance;
    float half_h_side = half_v_side * camera->aspect_ratio;

    camera->frustum.left_plane[0] = camera->view_projection_matrix[0][3] +
                                    camera->view_projection_matrix[0][0];
    camera->frustum.left_plane[1] = camera->view_projection_matrix[1][3] +
                                    camera->view_projection_matrix[1][0];
    camera->frustum.left_plane[2] = camera->view_projection_matrix[2][3] +
                                    camera->view_projection_matrix[2][0];
    camera->frustum.left_plane[3] = camera->view_projection_matrix[3][3] +
                                    camera->view_projection_matrix[3][0];
    plane_normalise(camera->frustum.left_plane);

    camera->frustum.right_plane[0] = camera->view_projection_matrix[0][3] -
                                     camera->view_projection_matrix[0][0];
    camera->frustum.right_plane[1] = camera->view_projection_matrix[1][3] -
                                     camera->view_projection_matrix[1][0];
    camera->frustum.right_plane[2] = camera->view_projection_matrix[2][3] -
                                     camera->view_projection_matrix[2][0];
    camera->frustum.right_plane[3] = camera->view_projection_matrix[3][3] -
                                     camera->view_projection_matrix[3][0];
    plane_normalise(camera->frustum.right_plane);

    camera->frustum.bottom_plane[0] = camera->view_projection_matrix[0][3] +
                                      camera->view_projection_matrix[0][1];
    camera->frustum.bottom_plane[1] = camera->view_projection_matrix[1][3] +
                                      camera->view_projection_matrix[1][1];
    camera->frustum.bottom_plane[2] = camera->view_projection_matrix[2][3] +
                                      camera->view_projection_matrix[2][1];
    camera->frustum.bottom_plane[3] = camera->view_projection_matrix[3][3] +
                                      camera->view_projection_matrix[3][1];
    plane_normalise(camera->frustum.bottom_plane);

    camera->frustum.top_plane[0] = camera->view_projection_matrix[0][3] -
                                   camera->view_projection_matrix[0][1];
    camera->frustum.top_plane[1] = camera->view_projection_matrix[1][3] -
                                   camera->view_projection_matrix[1][1];
    camera->frustum.top_plane[2] = camera->view_projection_matrix[2][3] -
                                   camera->view_projection_matrix[2][1];
    camera->frustum.top_plane[3] = camera->view_projection_matrix[3][3] -
                                   camera->view_projection_matrix[3][1];
    plane_normalise(camera->frustum.top_plane);

    camera->frustum.near_plane[0] = camera->view_projection_matrix[0][3] +
                                    camera->view_projection_matrix[0][2];
    camera->frustum.near_plane[1] = camera->view_projection_matrix[1][3] +
                                    camera->view_projection_matrix[1][2];
    camera->frustum.near_plane[2] = camera->view_projection_matrix[2][3] +
                                    camera->view_projection_matrix[2][2];
    camera->frustum.near_plane[3] = camera->view_projection_matrix[3][3] +
                                    camera->view_projection_matrix[3][2];
    plane_normalise(camera->frustum.near_plane);

    camera->frustum.far_plane[0] = camera->view_projection_matrix[0][3] -
                                   camera->view_projection_matrix[0][2];
    camera->frustum.far_plane[1] = camera->view_projection_matrix[1][3] -
                                   camera->view_projection_matrix[1][2];
    camera->frustum.far_plane[2] = camera->view_projection_matrix[2][3] -
                                   camera->view_projection_matrix[2][2];
    camera->frustum.far_plane[3] = camera->view_projection_matrix[3][3] -
                                   camera->view_projection_matrix[3][2];
    plane_normalise(camera->frustum.far_plane);
}

bool camera_is_sphere_in_frustum(struct camera *camera, struct vec3d center,
                                 float radius) {
    return plane_distance(camera->frustum.left_plane, center) > -radius &&
           plane_distance(camera->frustum.right_plane, center) > -radius &&
           plane_distance(camera->frustum.bottom_plane, center) > -radius &&
           plane_distance(camera->frustum.top_plane, center) > -radius &&
           plane_distance(camera->frustum.near_plane, center) > -radius &&
           plane_distance(camera->frustum.far_plane, center) > -radius;
}
