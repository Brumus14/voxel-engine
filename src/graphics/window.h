#ifndef WINDOW_H
#define WINDOW_H

#include "renderer.h"
#include "GLFW/glfw3.h"
#include <stdbool.h>
#include "camera.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"

struct window;

typedef void (*window_resize_callback)(struct window *, int, int);
typedef void (*window_cursor_position_callback)(struct window *, double,
                                                double);
typedef void (*window_scroll_callback)(struct window *, double, double);

struct window {
    GLFWwindow *glfw_window;
    int width;
    int height;
    char *title;
    struct camera *camera;
    struct keyboard keyboard;
    struct mouse mouse;
    double delta_time;
    double previous_time;
    window_resize_callback framebuffer_size_callback;
    window_cursor_position_callback cursor_pos_callback;
    window_scroll_callback scroll_callback;
};

void window_init(struct window *window, int width, int height, char *title,
                 struct camera *camera);
void window_destroy(struct window *window);
void window_shutdown();
bool window_should_close(struct window *window);
void window_swap_buffers(struct window *window);
float window_get_aspect_ratio(struct window *window);
void window_reset_cursor(struct window *window);
void window_capture_cursor(struct window *window);
void window_set_framebuffer_size_callback(struct window *window,
                                          window_resize_callback function);
void window_set_cursor_pos_callback(struct window *window,
                                    window_cursor_position_callback function);
void window_set_scroll_callback(struct window *window,
                                window_scroll_callback function);
double window_get_delta_time(struct window *window);
void window_update_delta_time(struct window *window);
void window_update_input(struct window *window);

#endif
