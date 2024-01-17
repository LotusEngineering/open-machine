// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "user_interface.h"
#include "om.h"
#include "board.h"
#include "shared_signals.h"


// If there are no button presses in config mode for this time, go back to idle
#define CONFIG_INACTIVITY_TIMEOUT_MSEC  10000

// Private signals
enum UiSignals
{
    EVT_STATUS_TIMEOUT = EVT_MAX_SHARED,
};


// Declare Init trans
OmStateResult ui_init_trans(UserInterface* self);

OM_STATE_DECLARE(UserInterface, ui_idle, OM_TOP_STATE);
OM_STATE_DECLARE(UserInterface, ui_brewing, OM_TOP_STATE);
OM_STATE_DECLARE(UserInterface, ui_done, OM_TOP_STATE);
OM_STATE_DECLARE(UserInterface, ui_config, OM_TOP_STATE);
    OM_STATE_DECLARE(UserInterface, ui_black, &ui_config);
    OM_STATE_DECLARE(UserInterface, ui_oolong, &ui_config);
    OM_STATE_DECLARE(UserInterface, ui_green, &ui_config);



void ui_ctor(UserInterface* self, OmBus* button_bus, OmActor* brew_control,  OmTrace* trace)
{
    // Call base actor trace constructor, only show transitions
    om_actor_ctor_trace(&self->base, 
                        OM_INIT_CAST(ui_init_trans), 
                        "UI", 
                        trace, 
                        OM_TF_TRANS);

    om_timer_ctor(&self->status_timer, EVT_STATUS_TIMEOUT, "StatusTimeout", &self->base);

    self->button_bus = button_bus;
    self->brew_control = brew_control;

    // Default to green tea, of course!
    self->selected_tea = TEA_TYPE_GREEN;
}

OmStateResult ui_init_trans(UserInterface* self)
{
    om_bus_subscribe(self->button_bus, &self->base);
    OmStateResult result = OM_TRANS(ui_idle);

    return result; 
}

OM_STATE_DEFINE(UserInterface, ui_idle)
{
    OmStateResult result = OM_RES_IGNORED;
    static bool first_release = true;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_leds(BOARD_LED_ALL_OFF);
            result = OM_RES_HANDLED;
        break;
        case EVT_BUTTON_RELEASE:

            // Ignore first release on power up 
            if (!first_release)
            {

                // Start brew control
                om_actor_start(self->brew_control, priority + 1, 16, 128 * 8);

                // Send a brew request to the brew control
                BrewRequestEvent* request = brew_request_event_new(self->selected_tea, &self->base);
                om_actor_message(self->brew_control, (OmEvent *)(request));
                
                result = OM_TRANS(ui_brewing);
            }
            first_release = false;

        break;
        case EVT_BUTTON_HELD:
            switch(self->selected_tea)
            {
                case TEA_TYPE_BLACK:
                    result = OM_TRANS(ui_black);
                break;
                case TEA_TYPE_OOLONG:
                    result = OM_TRANS(ui_oolong);
                break;
                case TEA_TYPE_GREEN:
                    result = OM_TRANS(ui_green);
                break;
            }

        break;

        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(UserInterface, ui_brewing)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case EVT_BREW_COMPLETE:
            result = OM_TRANS(ui_done);
        break;
        case EVT_BREW_STATUS:
            self->status = OM_EVENT_CAST(BrewStatusEvent)->status;

            // Show increasing level of LEDs during brew
            if (self->status.percent_complete <= 33)
            {
                board_set_leds(BOARD_LED_RED);
            }
            else if (self->status.percent_complete <= 66)
            {
                board_set_leds(BOARD_LED_RED | BOARD_LED_YELLOW);
            }
            else
            {
                board_set_leds(BOARD_LED_ALL_ON);
            }

            // Start status timer
            om_timer_start(&self->status_timer, OM_TM_ONE_SHOT, 500);

            result = OM_RES_HANDLED;
        break; 

        case EVT_STATUS_TIMEOUT:
            board_set_leds(BOARD_LED_ALL_OFF);
            result = OM_RES_HANDLED;
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(UserInterface, ui_done)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_leds(BOARD_LED_ALL_ON);
            result = OM_RES_HANDLED;
        break;
        case EVT_BREW_COMPLETE:
            result = OM_TRANS(ui_done);
        break;
        case EVT_BUTTON_RELEASE:
            result = OM_TRANS(ui_idle);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}


OM_STATE_DEFINE(UserInterface, ui_config)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            om_timer_start(&self->status_timer, OM_TM_ONE_SHOT, CONFIG_INACTIVITY_TIMEOUT_MSEC);
            result = OM_RES_HANDLED;
        break;
        case OM_EVT_EXIT:
            om_timer_stop(&self->status_timer);
            result = OM_RES_HANDLED;
        break;
        case EVT_STATUS_TIMEOUT:
            result = OM_TRANS(ui_idle);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(UserInterface, ui_black)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_leds(BOARD_LED_RED);
            self->selected_tea = TEA_TYPE_BLACK;
            result = OM_RES_HANDLED;
        break;
        case EVT_BUTTON_PRESS:
            om_timer_start(&self->status_timer, OM_TM_ONE_SHOT, CONFIG_INACTIVITY_TIMEOUT_MSEC);
            result = OM_TRANS(ui_oolong);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;

}

OM_STATE_DEFINE(UserInterface, ui_oolong)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_leds(BOARD_LED_YELLOW);
            self->selected_tea = TEA_TYPE_OOLONG;
            result = OM_RES_HANDLED;
        break;
        case EVT_BUTTON_PRESS:
            om_timer_start(&self->status_timer, OM_TM_ONE_SHOT, CONFIG_INACTIVITY_TIMEOUT_MSEC);
            result = OM_TRANS(ui_green);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;

}

OM_STATE_DEFINE(UserInterface, ui_green)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            board_set_leds(BOARD_LED_GREEN);
            self->selected_tea = TEA_TYPE_GREEN;
            result = OM_RES_HANDLED;
        break;
        case EVT_BUTTON_PRESS:
            om_timer_start(&self->status_timer, OM_TM_ONE_SHOT, CONFIG_INACTIVITY_TIMEOUT_MSEC);
            result = OM_TRANS(ui_black);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}
