#include "brew_control.h"
#include "om.h"
#include "shared_signals.h"

OM_ASSERT_SET_FILE_NAME();

#define BREW_TIME_BLACK_SEC     240
#define BREW_TIME_OOLONG_SEC    120
#define BREW_TIME_GREEN_SEC     180


// Private signals
enum BrewControlSignals
{
    EVT_ONE_SECOND = EVT_MAX_SHARED,
    EVT_BREW_START_REQ, 
};


typedef struct 
{
    OmPoolEvent base;
    TeaType tea_type;
    OmActor* client;
}BrewRequestEvent;

// Create Static Events
OM_TIME_EVENT(OneSecondEvent, EVT_ONE_SECOND);
OM_EVENT(BrewCompleteEvent, EVT_BREW_COMPLETE);

// Declare states
static OmStateResult brew_control_init_trans(BrewControl* self);
OM_STATE_DECLARE(BrewControl, brew_idle, OM_TOP_STATE);
OM_STATE_DECLARE(BrewControl, brew_steeping, OM_TOP_STATE);


void brew_control_ctor(BrewControl* self,  OmTrace* trace)
{
    // Call base actor trace constructor, only show transitions
    om_actor_ctor_trace(&self->base, OM_INIT_CAST(brew_control_init_trans), "Brew Control", trace, OM_TF_TRANS);

    // Create steep timer
    om_timer_ctor(&self->steep_timer, &self->base, OM_TIMER_PERIODIC, &OneSecondEvent);
   
}


void brew_control_send_start_request(BrewControl* self, TeaType tea_type, OmActor* client)
{
    BrewRequestEvent* req_evt = OM_POOL_EVENT_NEW(BrewRequestEvent,  EVT_BREW_START_REQ);
    req_evt->tea_type = tea_type;
    req_evt->client = client;
    om_actor_message(&self->base, &req_evt->base.base);
}

BrewStatusEvent* ibrew_status_event_new(BrewStatus const * const brew_status)
{
    BrewStatusEvent* status_event = OM_POOL_EVENT_NEW(BrewStatusEvent,  EVT_BREW_STATUS);
    status_event->status = *brew_status;
    return status_event;
}

OmStateResult brew_control_init_trans(BrewControl* self)
{
    OmStateResult result = OM_TRANS(brew_idle);
    return result;
}

int brew_control_get_brew_time_(BrewControl* self)
{
    int time_sec = 0;
    switch (self->tea_type)
    {
        case TEA_TYPE_BLACK:
            time_sec = BREW_TIME_BLACK_SEC;
        break;
        case TEA_TYPE_OOLONG:
            time_sec = BREW_TIME_OOLONG_SEC;
        break;
        case TEA_TYPE_GREEN:
            time_sec = BREW_TIME_GREEN_SEC;
        break;
        default:
            OM_ASSERT(0); // Invalid tea type
        break;
    }

    return time_sec;
}


OM_STATE_DEFINE(BrewControl, brew_idle)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case EVT_BREW_START_REQ:
            BrewRequestEvent const * const req_evt = OM_EVENT_CAST(BrewRequestEvent);
            self->tea_type = req_evt->tea_type;
            self->client = req_evt->client;
            result = OM_TRANS(brew_steeping);
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;
}

OM_STATE_DEFINE(BrewControl, brew_steeping)
{
    OmStateResult result = OM_RES_IGNORED;

    switch(event->signal)
    {
        case OM_EVT_ENTER:
            // Set the steeping time based on type
            self->steeping_time_sec = brew_control_get_brew_time_(self);

            // Start steep timer
            om_timer_start(&self->steep_timer, 1000);
            result = OM_RES_HANDLED;
        break;
        case EVT_ONE_SECOND:
            self->steeping_time_sec--;

            // Send status
            BrewStatus status;
            int total_time_sec = brew_control_get_brew_time_(self);
            status.percent_complete =  ((total_time_sec - self->steeping_time_sec) * 100)/total_time_sec ;
            status.tea_type = self->tea_type;
            BrewStatusEvent* status_event = ibrew_status_event_new(&status);
            om_actor_message(self->client, (OmEvent*)status_event);

            if (self->steeping_time_sec == 0)
            {

                om_actor_message(self->client, (OmEvent*)&BrewCompleteEvent);
                result = OM_TRANS(brew_idle);
            }
        break;
        default:
            result = OM_RES_IGNORED;
        break;
    }

    return result;

}
