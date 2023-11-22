// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <pthread.h>
#include <limits.h>
#include <stdlib.h>

#include "om_assert.h"
#include "om_machine.h"
#include "om_config.h"
#include "om_actor.h"
#include "om_timer.h"
#include "om_pool.h"
#include "om_queue.h"

OM_ASSERT_SET_FILE_NAME();

/**
 * @brief Actor Port definition for POSIX
 * 
 */
typedef struct OmActorPort
{
    pthread_t thread_id;
    OmQueue queue;
    OmEvent** queue_storage;
}OmActorPort;

static OmActorPort om_actor_table[OM_ACTOR_MAX_ACTORS];
static int om_actor_count = 0;

static void * om_actor_event_loop(void* argument);


void om_actor_ctor(OmActor* self, OmInitHandler initial_trans)
{
    om_actor_ctor_trace(self, initial_trans, NULL, NULL, OM_TF_NONE);
}

void om_actor_ctor_trace(OmActor * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags)
{
    // Call base machine trace constructor
    om_ctor_trace(&self->base, initial_trans, name, trace, flags);

    // Set Actor's trace
    self->trace = trace;

    self->port = &om_actor_table[om_actor_count];
    om_actor_count++;

    // Too many actors, update config, or better yet simplify your design
    OM_ASSERT(om_actor_count <= OM_ACTOR_MAX_ACTORS);
    self->port->thread_id = (pthread_t)NULL;
}

void om_actor_start(OmActor* self, int priority, size_t queue_size, uint32_t stack_size)
{
    // Allocate and init queue
    self->port->queue_storage = (OmEvent**)malloc(queue_size * sizeof(OmEvent*));
    OM_ASSERT(self->port->queue_storage != NULL);
    om_queue_init(&self->port->queue, self->port->queue_storage,  queue_size);


    pthread_attr_t attr;
    pthread_attr_init(&attr);
#if 0
    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    // Set priority
    struct sched_param param;
    param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);

#else
    struct sched_param param;
    pthread_attr_getschedparam (&attr, &param);
    param.sched_priority = priority;

    /* setting the new scheduling param */
    pthread_attr_setschedparam (&attr, &param);

#endif

    // Use minimum stack
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

    pthread_t thread;
    int err = pthread_create(&self->port->thread_id, &attr, om_actor_event_loop, self);
    OM_ASSERT(err == 0);

	pthread_attr_destroy(&attr);

}

void om_actor_stop(OmActor* self)
{
    //TODO graceful shutdown by clearing queue and posting
    pthread_cancel(self->port->thread_id);

     // Wait for the created thread to finish
    pthread_join(self->port->thread_id, NULL);
    free(self->port->queue_storage);
}

void om_actor_message(OmActor* self, OmEvent *  message)
{

    OM_ASSERT(self != NULL);

    // Increase reference count for pooled events
    if(message->type == OM_ET_POOL)
    {
        OM_POOL_EVENT_CAST(message)->reference_count++;        
    }

    // TODO signal mutex
    OM_ASSERT(om_queue_put(&self->port->queue, message));
}

//////////////////// Private Functions //////////////////////////
void *  om_actor_event_loop(void* argument)
{
    OmEvent const * event;
    OmActor* self = (OmActor*)argument;
    
    // Enter the state machine
    om_enter(&self->base);

    while(1)
    {
        //TODO Block on messages
        if (om_queue_get(&self->port->queue, &event))
        {
            if(event->type == OM_ET_TIME)
            {
                // Check for stale time events
                if (OM_TIME_EVENT_CAST(event)->state != OM_TS_STOPPED)
                {
                    om_dispatch(&self->base, event);
                }
            }
            else if(event->type == OM_ET_POOL)
            {
                // Check for memory pool events
                OmPoolEvent* pool_event = OM_POOL_EVENT_CAST(event);
                
                // Should have been incremented in om_actor_message()
                OM_ASSERT(pool_event->reference_count >= 1);

                om_dispatch(&self->base, event);
                
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
                om_dispatch(&self->base, event);
            }
        }// if
               
    }// while
}
