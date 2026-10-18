#include "../mutex.h"

#include <pthread.h>

void mutex_init(struct mutex *mutex) {
    InitializeSRWLock(&mutex->lock);
}

void mutex_lock(struct mutex *mutex) {
    AcquireSRWLockExclusive(&mutex->lock);
}

void mutex_unlock(struct mutex *mutex) {
    ReleaseSRWLockExclusive(&mutex->lock);
}

void mutex_destroy(struct mutex *mutex) {
}
