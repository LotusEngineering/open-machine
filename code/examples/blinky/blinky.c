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
OM_STATE_DECLARE(Off, Blinky, OM_TOP_STATE);
OM_STATE_DECLARE(On, Blinky,  OM_TOP_STATE);


// Tracing suppoprt
#define TRACE_LIST_SIZE 12
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace trace;

// Blinky instances
Blinky blink_yellow;

void om_assert_handler(const char *file_name, int line)
{
    om_trace_write(&trace, "ASSERT! File: %s, Line: %d", file_name, line);
}



void blinky_init(void)
{
    // Init tracing
    om_trace_ctor(&trace, trace_buffer, TRACE_LIST_SIZE);

    // Create actor
    blinky_ctor(&blink_yellow, &trace);

    // Start actors
    om_actor_start(&blink_yellow.base, 24, 16, 128 * 8);

 //   om_actor_post(&blink_yellow.base, &Event_TIMEOUT);
}


void blinky_ctor(Blinky* self, OmTrace* trace)
{
    // Call base actor constructor
    om_actor_ctor(&self->base, OM_INIT_CAST(blinky_init_trans), "Blinky", trace);

    om_timer_ctor(&self->timer, &self->base, OM_TIMER_PERIODIC, &Event_TIMEOUT);
}

OmStateResult blinky_init_trans(Blinky* self)
{
    // Start timer
    om_timer_start(&self->timer, 1000);

    OmStateResult result = OM_TRANS(Off);

    return result;
}

OM_STATE_DEFINE(Off, Blinky)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case EVT_TIMEOUT:
            result = OM_TRANS(On);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(On, Blinky)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case EVT_TIMEOUT:
            result = OM_TRANS(Off);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

