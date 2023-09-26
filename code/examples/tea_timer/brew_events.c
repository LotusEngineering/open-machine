// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "brew_events.h"
#include "om_actor.h"
#include "shared_signals.h"


BrewRequestEvent* brew_request_event_new(TeaType tea_type, OmActor* client)
{
    BrewRequestEvent* req_event = OM_POOL_EVENT_NEW(BrewRequestEvent,  EVT_BREW_START_REQ);
    req_event->tea_type = tea_type;
    req_event->client = client;
    return req_event;
}


BrewStatusEvent* brew_status_event_new(BrewStatus const * const brew_status)
{
    BrewStatusEvent* status_event = OM_POOL_EVENT_NEW(BrewStatusEvent,  EVT_BREW_STATUS);
    status_event->status = *brew_status;
    return status_event;
}
