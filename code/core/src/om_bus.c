// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om_bus.h"
#include "om_assert.h"

OM_ASSERT_SET_FILE_NAME("om_bus.c");

void om_bus_init(OmBus* self)
{
    om_mutex_init(&self->mutex);

    self->subscriber_count = 0;
    for (int i = 0; i < OM_ACTOR_MAX_ACTORS; i++)
    {
        self->subscribers[i]  = NULL;
    }
}

void om_bus_subscribe(OmBus* self, OmActor* subscriber)
{
    om_mutex_lock(&self->mutex);

    OM_ASSERT(self->subscriber_count <OM_ACTOR_MAX_ACTORS);

    for (int i = 0; i < OM_ACTOR_MAX_ACTORS; i++)
    {
        // Look for empty spot
        if (self->subscribers[i] == NULL)
        {
            self->subscribers[i] = subscriber;
            self->subscriber_count++;
            break;
        }   
    }
    om_mutex_unlock(&self->mutex);
}

void om_bus_unsubscribe(OmBus* self, OmActor* subscriber)
{
    om_mutex_lock(&self->mutex);
    // Look for subscriber
    for (int i = 0; i < OM_ACTOR_MAX_ACTORS; i++)
    {
        // Found subscriber?
        if (self->subscribers[i] == subscriber)
        {
            self->subscribers[i] = NULL;
            self->subscriber_count--;
            break;
        }   
    }
    om_mutex_unlock(&self->mutex);

}

bool om_bus_publish(OmBus* self, OmEvent * event)
{
    om_mutex_lock(&self->mutex);

    int published_count = 0;
    while (published_count < self->subscriber_count)
    {
        for (int i = 0; i < OM_ACTOR_MAX_ACTORS; i++)
        {
            // Message subscribers
            if (self->subscribers[i] != NULL)
            {
                om_actor_message(self->subscribers[i], event); 
                published_count++;
                if (published_count == self->subscriber_count)
                {
                    break;
                }
            }   
        }
    }

    if (published_count == 0)
    {
        om_mutex_unlock(&self->mutex);
        return false;

#if 0
        ///@TODO
        if ((event->type == OM_ET_POOL) && (OM_POOL_EVENT_CAST(event)->reference_count == 0))
        {
            om_pool_free(OM_POOL_EVENT_CAST(event));
        }        
#endif
    }
    else
    {
        om_mutex_unlock(&self->mutex);
        return true;
    }
}
