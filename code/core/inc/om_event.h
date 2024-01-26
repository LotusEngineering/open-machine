// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_EVENT_H_
#define OM_EVENT_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/// @brief Event signals reserved by OM
enum OmReservedSignals
{
    OM_EVT_ENTER,  ///< State Entry Action
    OM_EVT_INIT,   ///< Initial transition
    OM_EVT_EXIT,   ///< State Exit Action
    OM_EVT_ACTOR_STOP, ///< Event used to stop actor thread
    OM_EVT_USER,   ///< Start of user defined event signals
};

/// @brief Defines size of a signal
typedef uint32_t OmSignal;

/// @brief Defines the base types of events 
typedef enum 
{
    OM_ET_STATIC,
    OM_ET_POOL, ///< Dynamically allocated from a memory pool
    OM_ET_TIME, ///< Event used with timers
}OmEventType;

/// @brief Base OM event type
typedef struct 
{
    OmEventType type;
    OmSignal signal;
    const char* name;
}OmEvent;


/// Helper macro for defining static const events
#define OM_EVENT(event_name_, signal_) static const OmEvent (event_name_) = {OM_ET_STATIC,  signal_, #signal_}  

/// Helper macro for casting an event into the derived type
#define OM_EVENT_CAST(event_type_) ((event_type_ const * const)(event))


#endif //OM_EVENT_H_