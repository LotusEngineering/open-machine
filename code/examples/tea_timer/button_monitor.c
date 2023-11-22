// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "button_monitor.h"
#include "shared_signals.h"
#include "om.h"
#include "board.h"


#define BUTTON_SCAN_TIME_MSEC 50
#define BUTTON_DEBOUNCE_TIME_MS 100
#define BUTTON_MON_DEBOUNCE_LIMIT (BUTTON_DEBOUNCE_TIME_MS / BUTTON_SCAN_TIME_MSEC)
#define BUTTON_HELD_TIME_MSEC 2000

// Private signals
enum ButtonMonSignals
{
    EVT_SCAN_TIMEOUT = EVT_MAX_SHARED,
    EVT_HELD_TIMEOUT,
};

// Create events to publish on the button bus
OM_EVENT(PressedEvt, EVT_BUTTON_PRESS);
OM_EVENT(ReleasedEvt, EVT_BUTTON_RELEASE);
OM_EVENT(HeldEvt, EVT_BUTTON_HELD);


// Create timers
OM_TIME_EVENT(ScanTimeoutEvent, EVT_SCAN_TIMEOUT);
OM_TIME_EVENT(HeldTimeoutEvent, EVT_HELD_TIMEOUT);

// Declare Init trans
OmStateResult button_monitor_init_trans(ButtonMonitor* self);

// Declare the two states
OM_STATE_DECLARE(ButtonMonitor, Unknown, OM_TOP_STATE);
OM_STATE_DECLARE(ButtonMonitor, Pressed, OM_TOP_STATE);
OM_STATE_DECLARE(ButtonMonitor, Held, &Pressed);
OM_STATE_DECLARE(ButtonMonitor, Released, OM_TOP_STATE);



void button_monitor_ctor(ButtonMonitor* self, OmBus* button_bus, OmTrace* trace)
{
    self->debounce_counter = 0;
    self->button_bus = button_bus;

    // Call base actor trace constructor
    om_actor_ctor_trace(&self->base, OM_INIT_CAST(button_monitor_init_trans), "ButtonMonitor", trace, OM_TF_NONE);

    // Create periodic timer
    om_timer_ctor(&self->scan_timer, EVT_SCAN_TIMEOUT,"ScanTimeout", &self->base);

    // Create oneshot timer
    om_timer_ctor(&self->held_timer, EVT_HELD_TIMEOUT,"HeldTimeout",  &self->base);

}

OmStateResult button_monitor_init_trans(ButtonMonitor* self)
{
    // Start scan timer 
    om_timer_start(&self->scan_timer, OM_TM_PERIODIC, BUTTON_SCAN_TIME_MSEC);

    // At first, we don't know if we are pressed or not
    OmStateResult result = OM_TRANS(Unknown);

    return result;
}

OM_STATE_DEFINE(ButtonMonitor, Unknown)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case EVT_SCAN_TIMEOUT:
            if (board_button_is_down())
            {
                result = OM_TRANS(Pressed);
            }
            else
            {
                result = OM_TRANS(Released);
            }
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(ButtonMonitor, Pressed)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            self->debounce_counter = 0; 

            // Publish Pressed Event
            om_bus_publish(self->button_bus, (OmEvent*)(&PressedEvt));

            // Start Held Timer
            om_timer_start(&self->held_timer, OM_TM_ONE_SHOT, BUTTON_HELD_TIME_MSEC);
            
            result = OM_RES_HANDLED;
        break;
        case EVT_SCAN_TIMEOUT:
            if (!board_button_is_down())
            {
                self->debounce_counter++;
                if (self->debounce_counter > BUTTON_MON_DEBOUNCE_LIMIT)
                {
                    result = OM_TRANS(Released);
                }
            }
        break;
        case EVT_HELD_TIMEOUT:
            // Goto substate
            result = OM_TRANS(Held);
        break;
        case OM_EVT_EXIT:
            om_timer_stop(&self->held_timer);
            result = OM_RES_HANDLED;
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}
OM_STATE_DEFINE(ButtonMonitor, Held)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            // Publish Held Event
            om_bus_publish(self->button_bus, (OmEvent*)(&HeldEvt));

            result = OM_RES_HANDLED;
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(ButtonMonitor, Released)
{
    OmStateResult result = OM_RES_IGNORED;
    switch(event->signal)
    {
        case OM_EVT_ENTER:
            // Publish Released Event
            om_bus_publish(self->button_bus, (OmEvent*)(&ReleasedEvt));

            self->debounce_counter = 0; 
            result = OM_RES_HANDLED;
        break;
        case EVT_SCAN_TIMEOUT:
            if (board_button_is_down())
            {
                self->debounce_counter++;
                if (self->debounce_counter > BUTTON_MON_DEBOUNCE_LIMIT)
                {
                    result = OM_TRANS(Pressed);
                }
            }
        break;
       default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}