#include "mouse.h"

void mouse_init(struct mouse *mouse) {
    for (int i = 0; i < MOUSE_BUTTON_LAST; i++) {
        mouse->buttons[i] = BUTTON_STATE_UP;
        mouse->buttons_just_up[i] = false;
        mouse->buttons_just_down[i] = false;
    }

    mouse->position = VEC2D_ZERO;
    mouse->position_delta = VEC2D_ZERO;
    mouse->scroll_offset = VEC2D_ZERO;
}

void mouse_update_state(struct mouse *mouse) {
    mouse->position_delta = VEC2D_ZERO;
    mouse->scroll_offset = VEC2D_ZERO;

    for (int i = 0; i < MOUSE_BUTTON_LAST; i++) {
        mouse->buttons_just_up[i] = false;
        mouse->buttons_just_down[i] = false;
    }
}

void mouse_set_button(struct mouse *mouse, enum mouse_button button,
                      enum button_state state) {
    if (state == BUTTON_STATE_UP &&
        mouse->buttons[button] == BUTTON_STATE_DOWN) {
        mouse->buttons_just_up[button] = true;
    } else if (state == BUTTON_STATE_DOWN &&
               mouse->buttons[button] == BUTTON_STATE_UP) {
        mouse->buttons_just_down[button] = true;
    }

    mouse->buttons[button] = state;
}

enum button_state mouse_get_button(struct mouse *mouse,
                                   enum mouse_button button) {
    return mouse->buttons[button];
}

bool mouse_button_up(struct mouse *mouse, enum mouse_button button) {
    return (mouse->buttons[button] == BUTTON_STATE_UP);
}

bool mouse_button_just_up(struct mouse *mouse, enum mouse_button button) {
    return mouse->buttons_just_up[button];
}

bool mouse_button_down(struct mouse *mouse, enum mouse_button button) {
    return (mouse->buttons[button] == BUTTON_STATE_DOWN);
}

bool mouse_button_just_down(struct mouse *mouse, enum mouse_button button) {
    return mouse->buttons_just_down[button];
}

void mouse_set_position(struct mouse *mouse, struct vec2d position) {
    mouse->position_delta = vec2d_sub(position, mouse->position);
    mouse->position = position;
}
