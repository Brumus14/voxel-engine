#include "keyboard.h"

void keyboard_init(struct keyboard *keyboard) {
    for (int i = 0; i < KEYCODE_LAST; i++) {
        keyboard->keys[i] = KEY_STATE_UP;
        keyboard->keys_just_up[i] = false;
        keyboard->keys_just_down[i] = false;
    }
}

void keyboard_update_state(struct keyboard *keyboard) {
    for (int i = 0; i < KEYCODE_LAST; i++) {
        keyboard->keys_just_up[i] = false;
        keyboard->keys_just_down[i] = false;
    }
}

void keyboard_set_key(struct keyboard *keyboard, enum keycode key,
                      enum key_state state) {
    if (state == KEY_STATE_UP && keyboard->keys[key] == KEY_STATE_DOWN) {
        keyboard->keys_just_up[key] = true;
    } else if (state == KEY_STATE_DOWN && keyboard->keys[key] == KEY_STATE_UP) {
        keyboard->keys_just_down[key] = true;
    }

    keyboard->keys[key] = state;
}

enum key_state keyboard_get_key(struct keyboard *keyboard, enum keycode key) {
    return keyboard->keys[key];
}

bool keyboard_key_up(struct keyboard *keyboard, enum keycode key) {
    return (keyboard->keys[key] == KEY_STATE_UP);
}

bool keyboard_key_just_up(struct keyboard *keyboard, enum keycode key) {
    return (keyboard->keys_just_up[key] == true);
}

bool keyboard_key_down(struct keyboard *keyboard, enum keycode key) {
    return (keyboard->keys[key] == KEY_STATE_DOWN);
}

bool keyboard_key_just_down(struct keyboard *keyboard, enum keycode key) {
    return (keyboard->keys_just_down[key] == true);
}
