// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef BREW_EVENTS_H_
#define BREW_EVENTS_H_
#include "om_event.h"
#include "om_pool.h"
#include "om_actor.h"



typedef enum
{
    TEA_TYPE_BLACK,
    TEA_TYPE_OOLONG,
    TEA_TYPE_GREEN
}TeaType;

typedef struct 
{
    OmPoolEvent base;
    TeaType tea_type;
    OmActor* client;
}BrewRequestEvent;

typedef struct 
{
    TeaType tea_type;
    int percent_complete;
}BrewStatus;

typedef struct 
{
    OmPoolEvent base;
    BrewStatus status;
}BrewStatusEvent;


BrewRequestEvent* brew_request_event_new(TeaType tea_type, OmActor* client);


BrewStatusEvent* brew_status_event_new(BrewStatus const * const brew_status);


#endif