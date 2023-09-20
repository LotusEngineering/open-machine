#include "blinky.h"
#include "om.h"


// Define a signal and an event
enum BinkySignals
{
    EVT_TIMEOUT = OM_EVT_USER    
};

OM_EVT_DEFINE(TIMEOUT);

// Declare Init trans
OmStateResult blinky_init_trans(Blinky* self);

// Declare the two states
OM_STATE_DECLARE(Blinky, Off, OM_TOP_STATE);
OM_STATE_DECLARE(Blinky, On,  OM_TOP_STATE);


// Tracing suppoprt
#define TRACE_LIST_SIZE 12
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace blinky_trace;

// Blinky instances
Blinky blink_red;
Blinky blink_yellow;
Blinky blink_green;

void om_assert_handler(const char *file_name, int line)
{
    om_trace_write(&blinky_trace, "ASSERT! File: %s, Line: %d", file_name, line);
}



void blinky_actors_start(void)
{
    // Init tracing
    om_trace_ctor(&blinky_trace, trace_buffer, TRACE_LIST_SIZE);

    // Create actors, inject the LED they should blink
    blinky_ctor(&blink_red, "blink_red", &blinky_trace, BSP_LED_RED, 2000);
    blinky_ctor(&blink_yellow,"blink_yellow", &blinky_trace, BSP_LED_YELLOW, 1000);
    blinky_ctor(&blink_green, "blink_green", &blinky_trace, BSP_LED_GREEN, 500);

    // Start actors
    om_actor_start(&blink_yellow.base, 24, 16, 128 * 8);
    om_actor_start(&blink_red.base, 24, 16, 128 * 8);
    om_actor_start(&blink_green.base, 24, 16, 128 * 8);

}


void blinky_ctor(Blinky* self,const char* name, OmTrace* trace, Led_ID led_id, uint32_t blink_time_ms)
{
    // Call base actor trace constructor, only show transitions
    om_actor_ctor_trace(&self->base, OM_INIT_CAST(blinky_init_trans), name, trace, OM_TF_TRANS);

    om_timer_ctor(&self->timer, &self->base, OM_TIMER_PERIODIC, &Event_TIMEOUT);

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

