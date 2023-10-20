// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_TIMER_H_
#define OM_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "om_actor.h"
#include "om_event.h"


// Port specific data
typedef struct OmTimerPort OmTimerPort;


typedef enum
{
    OM_TIMER_CB_ACTOR,  ///< Timer will post event to actor
    OM_TIMER_CB_MACHINE, ///< Timer will dispatch event to machine
}OmTimerCallbackType;

typedef union 
{
    OmActor* actor;  
    OmMachine* machine;
}OmTimerCallback;

typedef enum
{
    OM_TS_STOPPED, 
    OM_TS_RUNNING, ///< Timer is counting down
    OM_TS_EXPIRED  ///< One shot timer expired
}OmTimerState;

typedef enum
{
    OM_TM_ONE_SHOT,
    OM_TM_PERIODIC
}OmTimerMode;

typedef struct 
{
    OmEvent base;
    OmActor* actor;
    OmTimerState state;
    OmTimerMode mode;
    uint32_t period_msec;
    uint32_t remaining_msec;
}OmTimer;





#define OM_TIME_EVENT_CAST(event_name_) ((OmTimer *)event_name_)

void om_timer_ctor(OmTimer* self, OmSignal signal, const char* name, OmActor* actor);

void om_timer_start(OmTimer* self, OmTimerMode mode, uint32_t time_msec);

bool om_timer_is_running(OmTimer* self);

void om_timer_stop(OmTimer* self);

void om_timer_tick(uint32_t elapsed_msec);


#endif //OM_TIMER_H_