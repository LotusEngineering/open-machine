// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_POOL_H_
#define OM_POOL_H_
#include <stdint.h>

#include "om_event.h"
#include "om_port.h"


// OS Port specific memory pool data
typedef struct OmPoolPort OmPoolPort;

/// @brief This is the base class for all dynamically allocated events
typedef struct 
{
    OmEvent base;
    OmPoolPort* port;
    uint32_t reference_count;
}OmPoolEvent;

/// Allocates a new event from the pool, NOTE: Event must derive from OmPoolEvent!
#define OM_POOL_EVENT_NEW(event_type_, signal_) (event_type_ *)(om_pool_alloc(sizeof(event_type_), signal_, #signal_))

/// Casts an event to a pool event
#define OM_POOL_EVENT_CAST(event_name_) ((OmPoolEvent *)event_name_)

/// @brief Initilizes memory pool system
/// @param  
void om_pool_init(void);


/// @brief Allocates a new event from a memory pool
/// @param event_size 
/// @param signal 
/// @param name 
/// @return 
OmPoolEvent* om_pool_alloc(size_t event_size, OmSignal signal, const char* name);


/// @brief Deallocates an event from the memory pool
/// @param event 
void om_pool_free(OmPoolEvent* event);

#endif //OM_POOL_H_