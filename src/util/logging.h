#define WORLD_LOGGING false
#define WORLD_LOG(s)     \
    if (WORLD_LOGGING) { \
        s;               \
    }
