// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "blinky.h"
#include "om.h"


// Define a signal and an event
enum BinkySignals
{
    EVT_TIMEOUT = OM_EVT_USER    
};

// Declare Init trans
OmStateResult blinky_init_trans(Blinky* self);

// Declare the two states
OM_STATE_DECLARE(Blinky, Off, OM_TOP_STATE);
OM_STATE_DECLARE(Blinky, On,  OM_TOP_STATE);


void blinky_init(Blinky* self,
                    Led_ID led_id,  
                    uint32_t blink_time_ms,  
                    OmActorAttr* actor_attr, 
                    OmTraceAttr* trace_attr) 

{
    // Call base actor trace init
    om_actor_init(&self->base, 
                    OM_INIT_CAST(blinky_init_trans),
                    actor_attr,
                    trace_attr);

    om_timer_init(&self->timer, EVT_TIMEOUT, "TIMEOUT", &self->base);

    self->led_id = led_id;
    self->blink_time_ms = blink_time_ms;
}

OmStateResult blinky_init_trans(Blinky* self)
{
    // Start timer
    om_timer_start(&self->timer, OM_TM_PERIODIC, self->blink_time_ms);

    OmStateResult result = OM_TRANS(Off);

    return result;
}

OM_STATE_DEFINE(Blinky, Off)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_led_off(self->led_id);
            result = OM_RES_HANDLED;
        break;
        case EVT_TIMEOUT:
            result = OM_TRANS(On);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(Blinky, On)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_led_on(self->led_id);
            result = OM_RES_HANDLED;
        break;
        case EVT_TIMEOUT:
            result = OM_TRANS(Off);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

