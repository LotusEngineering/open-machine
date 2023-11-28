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

typedef struct 
{
    OmEvent base;
    OmPoolPort* port;
    uint32_t reference_count;
}OmPoolEvent;


#define OM_POOL_EVENT_NEW(event_type_, signal_) (event_type_ *)(om_pool_alloc(sizeof(event_type_), signal_, #signal_))

#define OM_POOL_EVENT_CAST(event_name_) ((OmPoolEvent *)event_name_)


void om_pool_init(void);


/// @brief Allocates a new event from a memory pool
/// @param event_size 
/// @param signa 
/// @param name 
/// @return 
OmPoolEvent* om_pool_alloc(size_t event_size, OmSignal signa, const char* name);



void om_pool_free(OmPoolEvent* event);

//om_pool_gc()
#endif //OM_POOL_H_