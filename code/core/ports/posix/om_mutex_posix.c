#include "om_mutex.h"

void om_mutex_init(OmMutex* mutex)
{
    //TODO add asserts
    pthread_mutex_init(&mutex->port, NULL);
}

void om_mutex_lock(OmMutex* mutex)
{
    pthread_mutex_lock(&mutex->port); 
}

void om_mutex_unlock(OmMutex* mutex)
{
    pthread_mutex_unlock(&mutex->port); 
}