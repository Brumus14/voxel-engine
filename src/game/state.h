#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

struct state {
    bool paused;
};

void state_init(struct state *state);

#endif
