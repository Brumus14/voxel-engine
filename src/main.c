#include "glad/glad.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "graphics/camera.h"
#include "graphics/shader_program.h"
#include "graphics/window.h"
#include "input/keyboard.h"
#include "world/block.h"
#include "game/player.h"
#include "world/world.h"
#include "math/math_util.h"
#include "gui/gui.h"
#include "gui/image.h"
#include "gui/hotbar.h"
#include "game/items.h"
#include "game/state.h"
#include "game/main_gui.h"

// REMMEMBER TO AUTO BIND IN FUNCTIONS THAT ITS REQUIRED
// make arguments const

int main() {
    struct window window;
    struct camera camera;

    window_init(&window, 400, 400, "voxels!", &camera);

    renderer_init();
    renderer_set_clear_colour(0.53, 0.81, 0.92, 1.0);

    camera_init(&camera, VEC3D_ZERO, VEC3D_ZERO, 90.0,
                window_get_aspect_ratio(&window), 0.1, 1000.0);

    struct state state;
    state_init(&state);

    struct world world;
    world_init(&world);

    struct main_gui main_gui;
    main_gui_init(&main_gui, &window);

    // 21474836.0
    // 2147483.0
    struct player player;
    player_init(&player, (struct vec3d){8.0, 8.0, 8.0}, VEC3D_ZERO, 0.05,
                &camera);

    while (!window_should_close(&window)) {
        renderer_clear_buffers();

        window_update_delta_time(&window);
        window_update_input(&window);

        main_gui_update(&main_gui);

        printf("%f\n", 1.0 / window_get_delta_time(&window));

        static bool wireframe = false;
        if (keyboard_key_just_down(&window.keyboard, KEYCODE_P)) {
            wireframe = !wireframe;
            renderer_set_polygon_mode(wireframe ? POLYGON_MODE_LINE
                                                : POLYGON_MODE_FILL);
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_O)) {
            player.flying = !player.flying;
        }

        if (keyboard_key_just_down(&window.keyboard, KEYCODE_ESCAPE)) {
            state.paused = !state.paused;

            if (state.paused) {
                window_reset_cursor(&window);
            } else {
                window_capture_cursor(&window);
            }
        }

        if (!state.paused) {
            player_update(&player, &window, &world, &main_gui.hotbar);
        }

        world_prepare_draw(&world);
        camera_update(&camera);

        world_update(&world, player.position);
        world_draw(&world, &camera);

        renderer_clear_depth_buffer();

        main_gui_draw(&main_gui);

        window_swap_buffers(&window);
    }

    main_gui_destroy(&main_gui);
    world_destroy(&world);
    window_destroy(&window);

    window_shutdown();

    return 0;
}
