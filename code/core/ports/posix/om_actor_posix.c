// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <pthread.h>
#include <limits.h>
#include <stdlib.h>

#include "om_assert.h"
#include "om_hsm.h"
#include "om_config.h"
#include "om_actor.h"
#include "om_timer.h"
#include "om_pool.h"
#include "om_queue.h"

OM_ASSERT_SET_FILE_NAME("om_actor_posix.c");



static void * om_actor_event_loop(void* argument);

#if 0
void om_actor_ctor(OmActor* self, OmInitHandler initial_trans)
{
    om_actor_ctor_trace(self, initial_trans, NULL, NULL, OM_TF_NONE);
}

void om_actor_ctor_trace(OmActor * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags)
{
    // Call base machine trace constructor
    om_hsm_ctor_trace(&self->base, initial_trans, name, trace, flags);

    // Set Actor's trace
    self->trace = trace;

    self->port.thread_id = (pthread_t)NULL;

    pthread_mutex_init(&self->port.q_mutex, NULL);
    pthread_cond_init(&self->port.q_cond, NULL);
}
#endif
void om_actor_init(OmActor* const self,
                   OmInitHandler initial_trans, 
                   OmActorAttr* actor_attr,
                   OmTraceAttr* trace_attr )
{

    // Call base hsm init
    om_hsm_init(&self->base, initial_trans, trace_attr);

    // Store attributes
    self->priority = actor_attr->priority;
    self->queue_size = actor_attr->queue_size;
    self->stack_size = actor_attr->stack_size;

    pthread_mutex_init(&self->port.q_mutex, NULL);
    pthread_cond_init(&self->port.q_cond, NULL);

}

void om_actor_start(OmActor* self)
{
    // Allocate and init queue
    self->port.queue_storage = (OmEvent**)malloc(self->queue_size * sizeof(OmEvent*));
    OM_ASSERT(self->port.queue_storage != NULL);
    om_queue_init(&self->port.queue, self->port.queue_storage,  self->queue_size);


    pthread_attr_t attr;
    pthread_attr_init(&attr);
    struct sched_param param;
    pthread_attr_getschedparam (&attr, &param);
    param.sched_priority = self->priority;

    /* setting the new scheduling param */
    pthread_attr_setschedparam (&attr, &param);

    // Use minimum stack
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

    pthread_t thread;
    int err = pthread_create(&self->port.thread_id, &attr, om_actor_event_loop, self);
    OM_ASSERT(err == 0);

	pthread_attr_destroy(&attr);

}

void om_actor_stop(OmActor* self)
{
    // Send stop message to queue
    om_actor_send_stop_msg_(self);

     // Wait for the created thread to finish
    pthread_join(self->port.thread_id, NULL);
    free(self->port.queue_storage);
}

void om_actor_message(OmActor* self, OmEvent *  message)
{

    OM_ASSERT(self != NULL);

    // Increase reference count for pooled events
    if(message->type == OM_ET_POOL)
    {
        OM_POOL_EVENT_CAST(message)->reference_count++;        
    }

    pthread_mutex_lock(&self->port.q_mutex);

    // An assert here means we ran out of queue space
    OM_ASSERT(om_queue_put(&self->port.queue, message));

    // Signal that an event is waiting
    pthread_cond_signal(&self->port.q_cond);

    pthread_mutex_unlock(&self->port.q_mutex);

}

//////////////////// Private Functions //////////////////////////
void * om_actor_event_loop(void* argument)
{
    OmEvent * event;
    OmActor* self = (OmActor*)argument;
    
    // Enter the state machine
    om_hsm_enter(&self->base);

    while(om_hsm_is_active(&self->base))
    {
        pthread_mutex_lock(&self->port.q_mutex);
        while(!om_queue_get(&self->port.queue, &event))
        {
            pthread_cond_wait(&self->port.q_cond,	
                                &self->port.q_mutex);
        }
        pthread_mutex_unlock(&self->port.q_mutex);
        
        // Dispatch
        om_actor_dispatch_(self, event);
               
    }// while
}
