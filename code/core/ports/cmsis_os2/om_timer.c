
#include "om.h"
#include "om_config.h"
#include "cmsis_os2.h"


OM_ASSERT_SET_FILE_NAME();

/**
 * @brief Timer Port definition for CMSIS RTOS API V2
 * 
 */
typedef struct OmTimerPort
{
    osTimerId_t timer_id;
}OmTimerPort;

static OmTimerPort om_timer_table[OM_TIMER_MAX_TIMERS];
static int om_timer_count = 0;

static void om_timer_callback(void* argument);


void om_timer_ctor(OmTimer* self, OmActor* actor, OmTimerMode mode, OmEvent const * timeout_event)
{
    self->actor = actor;
    self->timeout_event = timeout_event;

    self->port = &om_timer_table[om_timer_count];
    om_timer_count++;

    // Too many timers
    OM_ASSERT(om_timer_count <= OM_TIMER_MAX_TIMERS);

    if (mode == OM_TIMER_ONE_SHOT)
    {
        self->port->timer_id = osTimerNew(om_timer_callback, osTimerOnce, (void *)self, NULL);
    }
    else
    {
        self->port->timer_id = osTimerNew(om_timer_callback, osTimerPeriodic, (void *)self, NULL);
    }
}

void om_timer_start(OmTimer* self, uint32_t time_ms)
{
    uint32_t tick_period_ms = 1000 / osKernelGetTickFreq();
    osStatus_t status = osTimerStart(self->port->timer_id, time_ms / tick_period_ms);
    OM_ASSERT(status == osOK);
}

bool om_timer_is_running(OmTimer* self)
{
    return osTimerIsRunning(self->port->timer_id);
}

void om_timer_stop(OmTimer* self)
{
    osTimerStop(self->port->timer_id);
}

void om_timer_callback(void* argument)
{
    OmTimer* self = (OmTimer*)argument;

    om_actor_post(self->actor, self->timeout_event);
}