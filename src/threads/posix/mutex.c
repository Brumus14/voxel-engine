#include "../mutex.h"
#include <pthread.h>

void mutex_init(struct mutex *mutex) {
    pthread_mutex_init(&mutex->mutex, NULL);
}

void mutex_lock(struct mutex *mutex) {
    pthread_mutex_lock(&mutex->mutex);
}

void mutex_unlock(struct mutex *mutex) {
    pthread_mutex_unlock(&mutex->mutex);
}

void mutex_destroy(struct mutex *mutex) {
    pthread_mutex_destroy(&mutex->mutex);
}
