#include "log.h"

struct mutex log_mutex;

void log_init() {
    mutex_init(&log_mutex);
}
