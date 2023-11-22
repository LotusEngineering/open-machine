// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_QUEUE_H_
#define OM_QUEUE_H_
#include <stddef.h>
#include <stdbool.h>
#include "om_event.h"

typedef struct
{
    OmEvent** store;
    size_t store_size;
    size_t write_index;
    size_t read_index;
}OmQueue;


void om_queue_ctor(OmQueue* self, OmEvent ** const queue_storage,  size_t queue_size);

bool om_queue_put(OmQueue* self, OmEvent const *const event);

bool om_queue_get(OmQueue* self, OmEvent const * event)

#endif //OM_QUEUE_H_