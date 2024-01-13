// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om.h"

OM_ASSERT_SET_FILE_NAME();

// This function is shared implementation amongst ports
void om_actor_dispatch_(OmActor* self, OmEvent* event)
{
    if(event->type == OM_ET_TIME)
    {
        // Check for stale time events
        if (OM_TIME_EVENT_CAST(event)->state != OM_TS_STOPPED)
        {
            om_hsm_dispatch(&self->base, event);
        }
    }
    else if(event->type == OM_ET_POOL)
    {
        // Check for memory pool events
        OmPoolEvent* pool_event = OM_POOL_EVENT_CAST(event);
        
        // Should have been incremented in om_actor_message()
        OM_ASSERT(pool_event->reference_count >= 1);

        om_hsm_dispatch(&self->base, event);
        
        // Decrement the reference count
        pool_event->reference_count--;

        // Free if count is zero
        if (pool_event->reference_count == 0)
        {
            om_pool_free(pool_event);
        }
    }
    else
    {
        om_hsm_dispatch(&self->base, event);
    }
}