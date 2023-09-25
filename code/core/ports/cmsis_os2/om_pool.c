#include "om_pool.h"
#include "om_config.h"
#include "om_assert.h"
#include "cmsis_os2.h"


OM_ASSERT_SET_FILE_NAME();

/**
 * @brief Memory Pool Port definition for CMSIS RTOS API V2
 * 
 */
typedef struct OmPoolPort
{
    osMemoryPoolId_t pool_id;
    size_t block_size;
}OmPoolPort;

static OmPoolPort om_pool_table[OM_POOL_NUM_POOLS];



void om_pool_init(void)
{
    if (OM_POOL_NUM_POOLS >= 1)
    {
        om_pool_table[0].pool_id = osMemoryPoolNew(OM_POOL_SMALL_BLOCK_COUNT, OM_POOL_SMALL_BLOCK_SIZE, NULL);
        om_pool_table[0].block_size = OM_POOL_SMALL_BLOCK_SIZE;
    }
    else
    {
        om_pool_table[0].pool_id = NULL;
    }
    
    if (OM_POOL_NUM_POOLS >= 2)
    {
        om_pool_table[1].pool_id = osMemoryPoolNew(OM_POOL_MEDIUM_BLOCK_COUNT, OM_POOL_MEDIUM_BLOCK_SIZE, NULL);
        om_pool_table[1].block_size = OM_POOL_MEDIUM_BLOCK_SIZE;
    }
    else
    {
        om_pool_table[1].pool_id = NULL;
    }

    if (OM_POOL_NUM_POOLS == 3)
    {
        om_pool_table[2].pool_id = osMemoryPoolNew(OM_POOL_LARGE_BLOCK_COUNT, OM_POOL_LARGE_BLOCK_SIZE, NULL);
        om_pool_table[2].block_size = OM_POOL_LARGE_BLOCK_SIZE;
    }
    else
    {
        om_pool_table[2].pool_id = NULL;
    }
}


OmPoolEvent* om_pool_alloc(size_t event_size, OmSignal signal, const char* name)
{
    OmPoolEvent* new_event = NULL;

    for(int idx = 0; idx < OM_POOL_NUM_POOLS; idx++)
    {
        if (event_size <= om_pool_table[idx].block_size)
        {
            // You must call om_pool_init
            OM_ASSERT(om_pool_table[idx].pool_id);

            new_event = osMemoryPoolAlloc(om_pool_table[idx].pool_id, 0); 

            // Could not allocate event, ran out of space?
            OM_ASSERT(new_event != NULL);

            new_event->base.type = OM_ET_POOL;
            new_event->base.signal = signal;
            new_event->base.name = name;
            new_event->reference_count = 0;
            new_event->port->pool_id = om_pool_table[idx].pool_id;

            break;
        }
    }

    // Could not find an event block, block size too big
    OM_ASSERT(new_event != NULL);

    return new_event;
}



void om_pool_free(OmPoolEvent* event)
{
    osStatus_t status = osMemoryPoolFree(event->port->pool_id, (void*)event);
    OM_ASSERT(status == osOK);
}
