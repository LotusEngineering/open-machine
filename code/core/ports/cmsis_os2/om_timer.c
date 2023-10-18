// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om.h"
#include "om_config.h"
#include "cmsis_os2.h"

#define USE_OS_TIMER 0

OM_ASSERT_SET_FILE_NAME();

/**
 * @brief Timer Port definition for CMSIS RTOS API V2
 * 
 */

#if USE_OS_TIMER
typedef struct OmTimerPort
{
    osTimerId_t timer_id;
}OmTimerPort;
#else
typedef struct OmTimerPort
{
    OmTimer* instance;
    OmTimerMode mode;
    uint32_t period_ms;
    uint32_t remaining_ms;
}OmTimerPort;
#endif

static OmTimerPort om_timer_table[OM_TIMER_MAX_TIMERS];
static int om_timer_count = 0;

static void om_timer_callback(void* argument);


void om_timer_ctor(OmTimer* self, OmActor* actor, OmTimerMode mode, OmTimeEvent* const time_event)
{
    self->actor = actor;
    
    // Time events must be used with timer
    OM_ASSERT(time_event->base.type == OM_ET_TIME);

    self->time_event = time_event;


    self->port = &om_timer_table[om_timer_count];
    om_timer_count++;

    // Too many timers
    OM_ASSERT(om_timer_count <= OM_TIMER_MAX_TIMERS);

#if USE_OS_TIMER

    if (mode == OM_TIMER_ONE_SHOT)
    {
        self->port->timer_id = osTimerNew(om_timer_callback, osTimerOnce, (void *)self, NULL);
    }
    else
    {
        self->port->timer_id = osTimerNew(om_timer_callback, osTimerPeriodic, (void *)self, NULL);
    }
#else
    self->port->instance = self;
    self->port->mode = mode;
#endif
}

void om_timer_start(OmTimer* self, uint32_t time_ms)
{
    self->time_event->is_running = true;

#if USE_OS_TIMER

    uint32_t tick_freq_hz = osKernelGetTickFreq();
    OM_ASSERT(tick_freq_hz != 0);
    OM_ASSERT(tick_freq_hz <= 1000000);

    uint32_t tick_period_us = 1000000 / tick_freq_hz;
    osStatus_t status = osTimerStart(self->port->timer_id, (time_ms * 1000) / tick_period_us);
    OM_ASSERT(status == osOK);
#else
    self->port->period_ms = time_ms;
    self->port->remaining_ms = time_ms;
#endif
}

bool om_timer_is_running(OmTimer* self)
{
#if USE_OS_TIMER
    return osTimerIsRunning(self->port->timer_id);
#else
    return self->time_event->is_running;
#endif
}

void om_timer_stop(OmTimer* self)
{
    // Mark the event as not running to avoid stale timer signals
    self->time_event->is_running = false;
#if USE_OS_TIMER
    osTimerStop(self->port->timer_id);
#else
#endif
}

void om_timer_tick(uint32_t elapsed_msec)
{
    for(int idx = 0; idx < om_timer_count; idx++)
    {
        OmTimerPort* port = &om_timer_table[idx];

        if (port->instance->time_event->is_running)
        {
            if(port->remaining_ms >= elapsed_msec)
            {
                port->remaining_ms -= elapsed_msec;
            }
            
            if(port->remaining_ms < elapsed_msec)
            {
                om_timer_callback(port->instance);
                if (port->mode == OM_TIMER_PERIODIC)
                {
                    port->remaining_ms = port->period_ms;
                }
                else
                {
                    port->instance->time_event->is_running = false;
                }
            }
        }
      
    }
}

void om_timer_callback(void* argument)
{
    OmTimer* self = (OmTimer*)argument;

    // If event is running send to actor
    if(self->time_event->is_running)
    {
        om_actor_message(self->actor, &self->time_event->base);
    }
}