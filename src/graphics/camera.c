#include "camera.h"

#include "glad/glad.h"
#include "../util/gl.h"
#include "../math/math_util.h"
#include "shader_program.h"

// use frustum culling
void generate_perspective_matrix(struct camera *camera) {
    if (!camera) {
        fprintf(stderr, "generate_perspective_matrix: struct camera is null\n");
        return;
    }

    glm_perspective(glm_rad(camera->fov), camera->aspect_ratio,
                    camera->near_plane, camera->far_plane,
                    camera->projection_matrix);
}

void generate_view_matrix(struct camera *camera) {
    if (!camera) {
        fprintf(stderr, "generate_view_matrix: struct camera is null\n");
        return;
    }

    struct vec3d direction = rotation_to_direction(camera->rotation);

    vec3 glm_direction;
    vec3d_to_glm(direction, &glm_direction);

    vec3 glm_position;
    vec3d_to_glm(camera->position, &glm_position);

    glm_look(glm_position, glm_direction, (vec3){0.0, 1.0, 0.0},
             camera->view_matrix);
}

void camera_init(struct camera *camera, struct vec3d position,
                 struct vec3d rotation, double fov, double aspect_ratio,
                 double near_plane, double far_plane) {
    if (!camera) {
        fprintf(stderr, "camera_init: struct camera is null\n");
        return;
    }

    vec3 glm_position;
    vec3d_to_glm(position, &glm_position);

    camera->position = position;
    camera->rotation = rotation;
    camera->fov = fov;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;

    generate_view_matrix(camera);
    generate_perspective_matrix(camera);

    shader_program_from_files(&camera->shader_program, "res/shaders/voxel.vert",
                              "res/shaders/voxel.frag");
    shader_program_bind_attribute(&camera->shader_program, 0, "position");
    shader_program_link(&camera->shader_program);
}

void camera_destroy(struct camera *camera) {
    shader_program_destroy(&camera->shader_program);
}

void camera_set_position(struct camera *camera, struct vec3d position) {
    camera->position = position;
}

// Relative to rotation
void camera_move(struct camera *camera, struct vec3d movement_delta) {
    if (!camera) {
        fprintf(stderr, "camera_translate: struct camera is null\n");
        return;
    }

    /*struct vec3d direction = direction_from_rotation(camera->rotation);*/
    struct vec3d direction = rotation_to_direction(camera->rotation);

    struct vec3d position_delta;
    vec3d_init(&position_delta, 0.0, 0.0, 0.0);

    struct vec3d forwards = direction;
    forwards.y = 0.0;
    vec3d_normalise(&forwards);

    position_delta = vec3d_add(
        position_delta, vec3d_scalar_multiply(forwards, -movement_delta.z));

    struct vec3d up;
    vec3d_init(&up, 0.0, 1.0, 0.0);

    position_delta =
        vec3d_add(position_delta,
                  vec3d_scalar_multiply(
                      vec3d_normalised(vec3d_cross_product(direction, up)),
                      movement_delta.x));

    position_delta.y += movement_delta.y;

    camera->position = vec3d_add(camera->position, position_delta);

    generate_view_matrix(camera);
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

    GLint view_loc =
        GL_CALL_R(glGetUniformLocation(shader_program_id, "view"), GLint);

    GL_CALL(glUniformMatrix4fv(
        view_loc, 1, GL_FALSE,
        (float *)camera->view_matrix)); // reduce casting for performance

    GLint projection_loc =
        GL_CALL_R(glGetUniformLocation(shader_program_id, "projection"), GLint);

    GL_CALL(glUniformMatrix4fv(projection_loc, 1, GL_FALSE,
                               (float *)camera->projection_matrix));

    // should this be moved somewhere else?
    mat4 model_matrix = GLM_MAT4_IDENTITY;
    GLint model_loc = GL_CALL(glGetUniformLocation(shader_program_id, "model"));
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

    generate_perspective_matrix(camera);
}

void camera_set_rotation(struct camera *camera, struct vec3d rotation) {
    if (!camera) {
        fprintf(stderr, "camera_set_rotation: struct camera is null\n");
        return;
    }

    camera->rotation = rotation;

    generate_view_matrix(camera);
}

void camera_set_fov(struct camera *camera, double fov) {
    if (!camera) {
        fprintf(stderr, "camera_set_rotation: struct camera is null\n");
        return;
    }

    camera->fov = fov;

    generate_perspective_matrix(camera);
}

void camera_rotate(struct camera *camera, struct vec3d rotation_delta) {
    if (!camera) {
        fprintf(stderr, "camera_rotate: struct camera is null\n");
        return;
    }

    struct vec3d new_rotation = vec3d_add(camera->rotation, rotation_delta);

    camera_set_rotation(camera, new_rotation);
}

void camera_prepare_draw(struct camera *camera) {
    shader_program_use(&camera->shader_program);
    camera_update_matrix_uniforms(camera);
}
