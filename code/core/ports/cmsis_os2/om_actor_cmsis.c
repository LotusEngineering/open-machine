// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "cmsis_os2.h"

#include "om_assert.h"
#include "om_hsm.h"
#include "om_config.h"
#include "om_actor.h"
#include "om_timer.h"
#include "om_pool.h"

OM_ASSERT_SET_FILE_NAME();



static void om_actor_event_loop(void* argument);


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

    // Set port values
    self->port.thread_attr.name = self->base.name;
    self->port.thread_attr.stack_size = 0;
    self->port.thread_attr.priority = (osPriority_t) osPriorityNormal;
    self->port.thread_id = NULL;
    self->port.queue_id = NULL; 
}

void om_actor_start(OmActor* self, int priority, size_t queue_size, uint32_t stack_size)
{
    self->port.thread_attr.stack_size = stack_size;
    self->port.thread_attr.priority = (osPriority_t) priority;
    self->port.queue_id = osMessageQueueNew(queue_size, sizeof(OmEvent*), NULL);
    OM_ASSERT(self->port.queue_id != NULL);

    self->port.thread_id = osThreadNew(om_actor_event_loop, (void *)self, &self->port.thread_attr);
    OM_ASSERT(self->port.thread_id != NULL);
}

void om_actor_stop(OmActor* self)
{
    //TODO graceful shutdown by clearing queue and posting 
    osThreadTerminate(self->port.thread_id);
    osThreadJoin(self->port.thread_id);
    osMessageQueueDelete(self->port.queue_id);
}

void om_actor_message(OmActor* self, OmEvent *  message)
{

    OM_ASSERT(self != NULL);
    OM_ASSERT(self->port.queue_id != NULL);

    // Increase reference count for pooled events
    if(message->type == OM_ET_POOL)
    {
        OM_POOL_EVENT_CAST(message)->reference_count++;        
    }

    osStatus_t status = osMessageQueuePut(self->port.queue_id, (void *)&message, 0, 0);
    OM_ASSERT(status == osOK);
}

//////////////////// Private Functions //////////////////////////
void om_actor_event_loop(void* argument)
{
    osStatus_t status;
    OmEvent * event;
    OmActor* self = (OmActor*)argument;
    
    // Enter the state machine
    om_hsm_enter(&self->base);

    while(1)
    {
        // Block on messages
        status = osMessageQueueGet(self->port.queue_id, &event, NULL, osWaitForever);   
        OM_ASSERT(status == osOK);
        
        // Dispatch
        om_actor_dispatch_(self, event);
               
    }// while
}
