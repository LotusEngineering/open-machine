// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <stdlib.h>

#include "om_pool.h"
#include "om_config.h"
#include "om_assert.h"


OM_ASSERT_SET_FILE_NAME();


static OmPoolPort om_pool_dummy;



void om_pool_init(void)
{

}


OmPoolEvent* om_pool_alloc(size_t event_size, OmSignal signal, const char* name)
{
    OmPoolEvent* new_event = NULL;


    new_event = (OmPoolEvent*)malloc(event_size);

    // Could not allocate event, ran out of space?
    OM_ASSERT(new_event != NULL);

    new_event->base.type = OM_ET_POOL;
    new_event->base.signal = signal;
    new_event->base.name = name;
    new_event->reference_count = 0;

    // Here we are not using a true memory pool
    new_event->port = &om_pool_dummy;

 
    return new_event;
}



void om_pool_free(OmPoolEvent* event)
{
    free((void*)event);
}
