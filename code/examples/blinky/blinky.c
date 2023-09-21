#include "blinky.h"
#include "om.h"


// Define a signal and an event
enum BinkySignals
{
    EVT_TIMEOUT = OM_EVT_USER    
};

OM_TIME_EVENT(TimeoutEvt, EVT_TIMEOUT);

// Declare Init trans
OmStateResult blinky_init_trans(Blinky* self);

// Declare the two states
OM_STATE_DECLARE(Blinky, Off, OM_TOP_STATE);
OM_STATE_DECLARE(Blinky, On,  OM_TOP_STATE);



void blinky_ctor(Blinky* self,const char* name, OmTrace* trace, Led_ID led_id, uint32_t blink_time_ms)
{
    // Call base actor trace constructor, only show transitions
    om_actor_ctor_trace(&self->base, OM_INIT_CAST(blinky_init_trans), name, trace, OM_TF_TRANS);

    om_timer_ctor(&self->timer, &self->base, OM_TIMER_PERIODIC, &TimeoutEvt);

    self->led_id = led_id;
    self->blink_time_ms = blink_time_ms;
}

OmStateResult blinky_init_trans(Blinky* self)
{
    // Start timer
    om_timer_start(&self->timer, self->blink_time_ms);

    OmStateResult result = OM_TRANS(Off);

    return result;
}

OM_STATE_DEFINE(Blinky, Off)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            bsp_set_led_off(self->led_id);
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
            bsp_set_led_on(self->led_id);
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

