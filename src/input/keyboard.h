#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

enum keycode {
    KEYCODE_0 = 0,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_8,
    KEYCODE_9,
    KEYCODE_A,
    KEYCODE_B,
    KEYCODE_C,
    KEYCODE_D,
    KEYCODE_E,
    KEYCODE_F,
    KEYCODE_G,
    KEYCODE_H,
    KEYCODE_I,
    KEYCODE_J,
    KEYCODE_K,
    KEYCODE_L,
    KEYCODE_M,
    KEYCODE_N,
    KEYCODE_O,
    KEYCODE_P,
    KEYCODE_Q,
    KEYCODE_R,
    KEYCODE_S,
    KEYCODE_T,
    KEYCODE_U,
    KEYCODE_V,
    KEYCODE_W,
    KEYCODE_X,
    KEYCODE_Y,
    KEYCODE_Z,
    KEYCODE_SPACE,
    KEYCODE_ESCAPE,
    KEYCODE_ENTER,
    KEYCODE_TAB,
    KEYCODE_BACKSPACE,
    KEYCODE_ARROW_RIGHT,
    KEYCODE_ARROW_LEFT,
    KEYCODE_ARROW_DOWN,
    KEYCODE_ARROW_UP,
    KEYCODE_LEFT_SHIFT,
    KEYCODE_LEFT_CONTROL,
    KEYCODE_LEFT_ALT,
    KEYCODE_LEFT_SUPER,
    KEYCODE_RIGHT_SHIFT,
    KEYCODE_RIGHT_CONTROL,
    KEYCODE_RIGHT_ALT,
    KEYCODE_RIGHT_SUPER,
    KEYCODE_LAST
};

enum key_state {
    KEY_STATE_UP = 0,
    KEY_STATE_DOWN,
};

struct keyboard {
    enum key_state keys[KEYCODE_LAST];
    bool keys_just_up[KEYCODE_LAST];
    bool keys_just_down[KEYCODE_LAST];
};

void keyboard_init(struct keyboard *keyboard);
void keyboard_update_state(struct keyboard *keyboard);
void keyboard_set_key(struct keyboard *keyboard, enum keycode key,
                      enum key_state state);
enum key_state keyboard_get_key(struct keyboard *keyboard, enum keycode key);
bool keyboard_key_up(struct keyboard *keyboard, enum keycode key);
bool keyboard_key_just_up(struct keyboard *keyboard, enum keycode key);
bool keyboard_key_down(struct keyboard *keyboard, enum keycode key);
bool keyboard_key_just_down(struct keyboard *keyboard, enum keycode key);

#endif
