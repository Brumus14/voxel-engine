#include "../condition.h"

void condition_init(struct condition *condition) {
    InitializeConditionVariable(&condition->condition);
}

void condition_wait(struct condition *condition, struct mutex *mutex) {
    SleepConditionVariableSRW(condition->condition, mutex->lock, INFINITE, 0);
}

void condition_signal(struct condition *condition) {
    WakeConditionVariable(&condition->condition);
}

void condition_broadcast(struct condition *condition) {
    WakeAllConditionVariable(&condition->condition);
}

void condition_destroy(struct condition *condition) {
}
