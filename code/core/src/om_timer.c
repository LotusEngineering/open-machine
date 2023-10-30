// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om.h"
#include "om_config.h"


OM_ASSERT_SET_FILE_NAME();


static OmTimer* om_timer_table[OM_TIMER_MAX_TIMERS];
static int om_timer_count = 0;


void om_timer_ctor(OmTimer* self, OmSignal signal, const char* name, OmActor* actor)
{
    om_timer_table[om_timer_count] = self;
    om_timer_count++;

    // Too many timers
    OM_ASSERT(om_timer_count <= OM_TIMER_MAX_TIMERS);
    
    self->base.type = OM_ET_TIME;
    self->base.signal = signal;
    self->base.name = name;
    self->callback_type = OM_TIMER_CB_ACTOR;
    self->callback.actor = actor;
    self->state = OM_TS_STOPPED;
}

void om_timer_machine_ctor(OmTimer* self, OmSignal signal, const char* name, OmMachine * machine)
{
    om_timer_table[om_timer_count] = self;
    om_timer_count++;

    // Too many timers
    OM_ASSERT(om_timer_count <= OM_TIMER_MAX_TIMERS);
    
    self->base.type = OM_ET_TIME;
    self->base.signal = signal;
    self->base.name = name;
    self->callback_type = OM_TIMER_CB_MACHINE;
    self->callback.machine = machine;
    self->state = OM_TS_STOPPED;
}

void om_timer_start(OmTimer* self, OmTimerMode mode, uint32_t time_msec)
{
    self->mode = mode;
    self->period_msec = time_msec;
    self->remaining_msec = time_msec;
    self->state = OM_TS_RUNNING;
}

bool om_timer_is_running(OmTimer* self)
{
    return self->state == OM_TS_RUNNING;
}

void om_timer_stop(OmTimer* self)
{
    self->state = OM_TS_STOPPED;
}

void om_timer_tick(uint32_t elapsed_msec)
{
    for(int idx = 0; idx < om_timer_count; idx++)
    {
        OmTimer* timer = om_timer_table[idx];

        if (timer->state == OM_TS_RUNNING)
        {
            // Count down
            if(timer->remaining_msec >= elapsed_msec)
            {
                timer->remaining_msec -= elapsed_msec;
            }
            
            // Timed out?
            if(timer->remaining_msec < elapsed_msec)
            {
                if (timer->mode == OM_TM_PERIODIC)
                {
                    timer->remaining_msec = timer->period_msec;
                }
                else
                {
                    timer->state = OM_TS_EXPIRED;
                }

                // Dispatch or message based on callback type
                if(timer->callback_type == OM_TIMER_CB_ACTOR)
                {
                    om_actor_message(timer->callback.actor, &timer->base);
                }
                else
                {
                    om_dispatch(timer->callback.machine, &timer->base);
                }
            }
        }
      
    }
}
