// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_BUS_H_
#define OM_BUS_H_

#include <stdbool.h>
#include "om_event.h"
#include "om_actor.h"
#include "om_config.h"
#include "om_mutex.h"

typedef struct 
{
    OmActor* subscribers[OM_ACTOR_MAX_ACTORS];
    int subscriber_count;
    OmMutex mutex;
}OmBus;

void om_bus_ctor(OmBus* self);

void om_bus_subscribe(OmBus* self, OmActor* subscriber);

void om_bus_unsubscribe(OmBus* self, OmActor* subscriber);

bool om_bus_publish(OmBus* self, OmEvent * event);

#endif // OM_BUS_H_