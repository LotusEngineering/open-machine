#include "om_mutex.h"
#include "om_assert.h"

OM_ASSERT_SET_FILE_NAME();

void om_mutex_init(OmMutex* mutex)
{
    &mutex->port = osMutexNew(NULL);
    OM_ASSERT(&mutex->port != NULL);
}

void om_mutex_lock(OmMutex* mutex)
{
    OM_ASSERT(osMutexAcquire(mutex_id, osWaitForever) == osOK);
}

void om_mutex_unlock(OmMutex* mutex)
{
    OM_ASSERT(osMutexRelease(mutex_id) == osOK);
}