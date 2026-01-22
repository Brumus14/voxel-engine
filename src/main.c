#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "math/vec3/vec3_convert.h"
#include "world/block.h"
#include "game/player.h"
#include "world/world.h"
#include "math/math_util.h"
#include "gui/gui.h"
#include "gui/image.h"
#include "gui/hotbar.h"
#include "game/items.h"

// REMMEMBER TO AUTO BIND IN FUNCTIONS THAT ITS REQUIRED
// make arguments const

int main() {
    struct window window;
    struct camera camera;
    struct gui gui;

    window_init(&window, 400, 400, "minecraft!", &camera);

    renderer_init();
    renderer_set_clear_colour(0.53, 0.81, 0.92, 1.0);
    // renderer_set_polygon_mode(POLYGON_MODE_LINE);

    camera_init(&camera, VEC3D_ZERO, VEC3D_ZERO, 90.0,
                window_get_aspect_ratio(&window), 0.1, 1000.0);

    gui_init(&gui, &window);

    struct gui_image crosshair;
    gui_image_init(&crosshair, "res/textures/crosshair.png", VEC2D_ZERO,
                   (struct vec2d){1, 1}, GUI_ELEMENT_ORIGIN_CENTER_CENTER,
                   GUI_ELEMENT_LAYER_0);

    gui_add_image(&gui, &crosshair);

    struct hotbar hotbar;
    hotbar_init(&hotbar, &gui);

    hotbar_set_item(&hotbar, 0, ITEM_TYPE_GRASS_BLOCK);
    hotbar_set_item(&hotbar, 1, ITEM_TYPE_DIRT_BLOCK);
    hotbar_set_item(&hotbar, 2, ITEM_TYPE_STONE_BLOCK);
    hotbar_set_item(&hotbar, 3, ITEM_TYPE_COAL_BLOCK);
    hotbar_set_item(&hotbar, 4, ITEM_TYPE_LOG_BLOCK);
    hotbar_set_item(&hotbar, 5, ITEM_TYPE_DIAMOND_BLOCK);

    struct world world;
    world_init(&world);

    world_load_chunk(&world, (struct vec3i){0, 0, 0});
    // world_load_chunk(&world, (struct vec3i){-1, 0, 0});
    // world_load_chunk(&world, (struct vec3i){-1, -1, 0});

    // 21474836.0
    // 2147483.0
    struct player player;
    player_init(&player, (struct vec3d){8.0, 4.0, 8.0}, VEC3D_ZERO, 0.05,
                &camera);

    while (!window_should_close(&window)) {
        renderer_clear_buffers();

        window_update_delta_time(&window);
        window_update_input(&window);

        if (window.mouse.scroll_offset.y < -0.5) {
            hotbar.current_slot++;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        } else if (window.mouse.scroll_offset.y > 0.5) {
            hotbar.current_slot--;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        gui_image_set_position(
            &crosshair,
            (struct vec2d){(double)window.width / 2,
                           (double)window.height /
                               2}); // only run when struct window size changed
        hotbar_update_gui(&hotbar);

        // printf("%f\n", 1.0 / window_get_delta_time(&window));

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_LEFT_CONTROL)) {
            player.sprinting = !player.sprinting;
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_ESCAPE)) {
            window_reset_cursor(&window);
        }

        if (mouse_button_just_down(&window.mouse, MOUSE_BUTTON_LEFT)) {
            window_capture_cursor(&window);
            player_destroy_block(&player, &world);
        }
        world_get_block(&world, vec3i_from_vec3d_floor(player.position));

        if (mouse_button_just_down(&window.mouse, MOUSE_BUTTON_RIGHT)) {
            enum block_type current_block =
                item_type_to_block_type(hotbar_get_current_item(&hotbar));

            if (current_block != -1) {
                if (keyboard_key_down(&window.keyboard, KEYCODE_R)) {
                    player_replace_block(&player, &world, current_block);
                } else {
                    player_place_block(&player, &world, current_block);
                }
            }
        }

        // move to hotbar file?
        if (keyboard_key_just_down(&window.keyboard, KEYCODE_1)) {
            // make into function so doesnt update hotbar gui every frame
            hotbar.current_slot = 0;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_2)) {
            hotbar.current_slot = 1;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_3)) {
            hotbar.current_slot = 2;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_4)) {
            hotbar.current_slot = 3;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_5)) {
            hotbar.current_slot = 4;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_6)) {
            hotbar.current_slot = 5;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_7)) {
            hotbar.current_slot = 6;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_8)) {
            hotbar.current_slot = 7;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_9)) {
            hotbar.current_slot = 8;
            hotbar.current_slot = mod(hotbar.current_slot, 9);
        }

        player_update(&player, &window, &world);

        camera_set_rotation(&camera, player.rotation);
        camera_set_position(
            &camera, vec3d_add(player.position, (struct vec3d){0, 0.6, 0}));

        if (player.sprinting) {
            if (camera.fov < 90 * 1.1) {
                camera_set_fov(
                    &camera, camera.fov + window_get_delta_time(&window) * 70);
            } else if (camera.fov > 90 * 1.1) {
                camera_set_fov(&camera, 90 * 1.1);
            }
        } else {
            if (camera.fov > 90) {
                camera_set_fov(
                    &camera, camera.fov - window_get_delta_time(&window) * 70);
            } else if (camera.fov < 90) {
                camera_set_fov(&camera, 90);
            }
        }

        camera_prepare_draw(&camera);

        world_draw(&world); // SLOW

        renderer_clear_depth_buffer();
        gui_draw(&gui);

        window_swap_buffers(&window);
    }

    window_destroy(&window);

    glfwTerminate(); // move this somewhere

    return 0;
}
