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

/// @brief Defines a event queue data type
typedef struct OmQueue_t
{
    OmEvent** store;
    size_t store_size;
    size_t write_index;
    size_t read_index;
}OmQueue;

/// @brief Initializes an event queue
/// @param self 
/// @param queue_storage 
/// @param queue_size 
void om_queue_init(OmQueue* self, OmEvent ** const queue_storage,  size_t queue_size);

/// @brief Puts an event into a queue
/// @param self 
/// @param event 
/// @return true if event was queued
bool om_queue_put(OmQueue* self, OmEvent const *const event);

/// @brief Gets a event from a queue
/// @param self 
/// @param event 
/// @return true if an event was returned
bool om_queue_get(OmQueue* self, OmEvent const ** event);

#endif //OM_QUEUE_H_