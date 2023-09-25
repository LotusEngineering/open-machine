#include "cmsis_os2.h"

#include "om_assert.h"
#include "om_machine.h"
#include "om_config.h"
#include "om_actor.h"
#include "om_timer.h"
#include "om_pool.h"

OM_ASSERT_SET_FILE_NAME();

/**
 * @brief Actor Port definition for CMSIS RTOS API V2
 * 
 */
typedef struct OmActorPort
{
    osThreadId_t thread_id;
    osThreadAttr_t thread_attr;
    osMessageQueueId_t queue_id;
}OmActorPort;

static OmActorPort om_actor_table[OM_ACTOR_MAX_ACTORS];
static int om_actor_count = 0;

static void om_actor_event_loop(void* argument);


void om_actor_ctor(OmActor* self, OmInitHandler initial_trans)
{
    om_actor_ctor_trace(self, initial_trans, NULL, NULL, OM_TF_NONE);
}

void om_actor_ctor_trace(OmActor * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags)
{
    // Call base machine trace constructor
    om_ctor_trace(&self->base, initial_trans, name, trace, flags);

    self->port = &om_actor_table[om_actor_count];
    om_actor_count++;

    // Too many actors, update config, or better yet simplify your design
    OM_ASSERT(om_actor_count <= OM_ACTOR_MAX_ACTORS);

    self->port->thread_attr.name = self->base.name;
    self->port->thread_attr.stack_size = 0;
    self->port->thread_attr.priority = (osPriority_t) osPriorityNormal;
    self->port->thread_id = NULL;
    self->port->queue_id = NULL; 
}

void om_actor_start(OmActor* self, int priority, size_t queue_size, uint32_t stack_size)
{
    self->port->thread_attr.stack_size = stack_size;
    self->port->thread_attr.priority = (osPriority_t) priority;
    self->port->queue_id = osMessageQueueNew(queue_size, sizeof(OmEvent*), NULL);
    OM_ASSERT(self->port->queue_id != NULL);

    self->port->thread_id = osThreadNew(om_actor_event_loop, (void *)self, &self->port->thread_attr);
    OM_ASSERT(self->port->thread_id != NULL);
}

void om_actor_stop(OmActor* self)
{
    //TODO graceful shutdown by clearing queue and posting 
    osThreadTerminate(self->port->thread_id);
    osThreadJoin(self->port->thread_id);
    osMessageQueueDelete(self->port->queue_id);
}

void om_actor_message(OmActor* self, OmEvent *  message)
{

    OM_ASSERT(self != NULL);

    // Increase reference count for pooled events
    if(message->type == OM_ET_POOL)
    {
        OM_POOL_EVENT_CAST(message)->reference_count++;        
    }

    osStatus_t status = osMessageQueuePut(self->port->queue_id, (void *)&message, 0, 0);
    OM_ASSERT(status == osOK);
}

//////////////////// Private Functions //////////////////////////
void om_actor_event_loop(void* argument)
{
    osStatus_t status;
    OmEvent const * event;
    OmActor* self = (OmActor*)argument;
    
    // Enter the state machine
    om_enter(&self->base);

    while(1)
    {
        // Block on messages
        status = osMessageQueueGet(self->port->queue_id, &event, NULL, osWaitForever);   
        OM_ASSERT(status == osOK);

        if(event->type == OM_ET_TIME)
        {
            // Check for stale time events
            if (OM_TIME_EVENT_CAST(event)->is_running)
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
               
    }// while
}
