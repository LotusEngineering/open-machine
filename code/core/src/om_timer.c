// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om.h"
#include "om_config.h"
#include "om_mutex.h"


OM_ASSERT_SET_FILE_NAME();


static OmTimer* om_timer_table[OM_TIMER_MAX_TIMERS];
static int om_timer_count = 0;
static OmMutex om_timer_mutex;


void om_timer_ctor(OmTimer* self, OmSignal signal, const char* name, OmActor* actor)
{
    // If this is the first timer constructed, init mutex
    if(om_timer_count == 0)
    {
        om_mutex_init(&om_timer_mutex);
    }
    om_mutex_lock(&om_timer_mutex);

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
    om_mutex_unlock(&om_timer_mutex);
}

void om_timer_ctor_hsm(OmTimer* self, OmSignal signal, const char* name, OmHsm * hsm)
{
    // If this is the first timer constructed, init mutex
    if(om_timer_count == 0)
    {
        om_mutex_init(&om_timer_mutex);
    }
    om_mutex_lock(&om_timer_mutex);

    om_timer_table[om_timer_count] = self;
    om_timer_count++;

    // Too many timers
    OM_ASSERT(om_timer_count <= OM_TIMER_MAX_TIMERS);
    
    self->base.type = OM_ET_TIME;
    self->base.signal = signal;
    self->base.name = name;
    self->callback_type = OM_TIMER_CB_HSM;
    self->callback.hsm = hsm;
    self->state = OM_TS_STOPPED;
    om_mutex_lock(&om_timer_mutex);
}

void om_timer_start(OmTimer* self, OmTimerMode mode, uint32_t time_msec)
{
    om_mutex_lock(&om_timer_mutex);
    self->mode = mode;
    self->period_msec = time_msec;
    self->remaining_msec = time_msec;
    self->state = OM_TS_RUNNING;
    om_mutex_unlock(&om_timer_mutex);
}

void om_timer_start_delayed(OmTimer* self, OmTimerMode mode, uint32_t time_msec, uint32_t delay_time_msec)
{
    om_mutex_lock(&om_timer_mutex);
    self->mode = mode;
    self->period_msec = time_msec;
    self->remaining_msec = time_msec + delay_time_msec;
    self->state = OM_TS_RUNNING;
    om_mutex_unlock(&om_timer_mutex);
}

bool om_timer_is_running(OmTimer* self)
{
    om_mutex_lock(&om_timer_mutex);
    bool running = self->state == OM_TS_RUNNING;
    om_mutex_unlock(&om_timer_mutex);
    return running;
}

void om_timer_stop(OmTimer* self)
{
    om_mutex_lock(&om_timer_mutex);
    self->state = OM_TS_STOPPED;
    om_mutex_unlock(&om_timer_mutex);
}

void om_timer_tick(uint32_t elapsed_msec)
{
    om_mutex_lock(&om_timer_mutex);
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
                    om_hsm_dispatch(timer->callback.hsm, &timer->base);
                }
            }
        }
      
    }
    om_mutex_unlock(&om_timer_mutex);

}
