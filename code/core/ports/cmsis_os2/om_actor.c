
#include "om_assert.h"
#include "om_machine.h"
#include "om_config.h"
#include "om_actor.h"
#include "cmsis_os2.h"

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

#if 0
OmActor* om_actor_new(OmMachine* machine)
{
    OmActor* new_actor =  &om_actor_table[om_actor_count];
    new_actor->machine = machine;

    om_actor_count++;

    // Too many actors, update config, or better yet simplify your design
    OM_ASSERT(om_actor_count < OM_ACTOR_MAX_ACTORS);

    return  new_actor;
}
#endif

void om_actor_ctor(OmActor* self, OmInitHandler initial_trans, const char* name, OmTrace* trace )
{
    // Call base machine constructor
    om_ctor(&self->base, initial_trans, name, trace);

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
    //TODO graceful shutdown
    osThreadTerminate(self->port->thread_id);
}

void om_actor_post(OmActor* self, OmEvent const * const event)
{
    osStatus_t status = osMessageQueuePut(self->port->queue_id, (void *)&event, 0, 0);
    OM_ASSERT(status == osOK);
}

//////////////////// Private Functions //////////////////////////
void om_actor_event_loop(void* argument)
{
    osStatus_t status;
    OmEvent const * event;

    OmActor* self = (OmActor*)argument;
    om_enter(&self->base);

    while(1)
    {
        status = osMessageQueueGet(self->port->queue_id, &event, NULL, 0U);   // wait for message
        if (status == osOK) 
        {
            om_dispatch(&self->base, event);
        }        

    }
}
