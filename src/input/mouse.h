#ifndef MOUSE_H
#define MOUSE_H

#include "../math/vec2.h"

enum mouse_button {
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_LAST
};

enum button_state {
    BUTTON_STATE_UP = 0,
    BUTTON_STATE_DOWN,
};

struct mouse {
    enum button_state buttons[MOUSE_BUTTON_LAST];
    bool buttons_just_up[MOUSE_BUTTON_LAST];
    bool buttons_just_down[MOUSE_BUTTON_LAST];
    struct vec2d position;
    struct vec2d position_delta;
    struct vec2d scroll_offset;
};

void mouse_init(struct mouse *mouse);
void mouse_update_state(struct mouse *mouse);
void mouse_set_button(struct mouse *mouse, enum mouse_button button,
                      enum button_state state);
enum button_state mouse_get_button(struct mouse *mouse,
                                   enum mouse_button button);
bool mouse_button_up(struct mouse *mouse, enum mouse_button button);
bool mouse_button_just_up(struct mouse *mouse, enum mouse_button button);
bool mouse_button_down(struct mouse *mouse, enum mouse_button button);
bool mouse_button_just_down(struct mouse *mouse, enum mouse_button button);
void mouse_set_position(struct mouse *mouse, struct vec2d position);

#endif
