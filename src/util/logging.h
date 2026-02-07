#define WORLD_LOGGING true
#define WORLD_LOG(s)     \
    if (WORLD_LOGGING) { \
        s;               \
    }
