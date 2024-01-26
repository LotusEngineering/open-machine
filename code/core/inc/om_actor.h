// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_ACTOR_H_
#define OM_ACTOR_H_
#include <stddef.h>
#include "om_hsm.h"
#include "om_port.h"



// Forward declare
//struct OmHsm;
struct OmState;
struct OmEvent;

// Forward declare OS Port specific actor data
typedef struct OmActorPort OmActorPort;

typedef struct 
{
    OmHsm base;
    int priority;
    size_t queue_size;
    uint32_t stack_size;
    OmActorPort port;
}OmActor;


/// @brief  Actor initialization attributes
typedef struct 
{
    int priority;
    size_t queue_size;
    uint32_t stack_size;
}OmActorAttr;

/// @brief Base class to be used by application code for sending requests
typedef struct 
{
    OmEvent base;
    OmActor* client;
}OmRequestEvent;

/// Helper macro to trace arbitray string
#define OMA_TRACE_STR(string_) om_trace_string(self->base.trace, string_)

/// Helper macro to trace a string variable
#define OMA_TRACE_VAL_STR(string_) OM_TRACE_VAL_STR(self->base.trace, string_)

/// Helper macro to trace an integer variable as a decimal
#define OMA_TRACE_VAL_DEC(string_) OM_TRACE_VAL_DEC(self->base.trace, string_)

/// Helper macro to trace an integer variable as a hexadecimal
#define OMA_TRACE_VAL_HEX(string_) OM_TRACE_VAL_HEX(self->base.trace, string_)

/// Helper macro to send an event message to an actor
#define OMA_MSG(target_, event_) om_actor_message((OmActor*)(target_), (OmEvent*)(event_))


/// @brief Actor initialization
/// @param self 
/// @param initial_trans Initial transition handler
/// @param actor_attr Actor attributes
/// @param trace_attr Trace attributes or NULL for no tracing
void om_actor_init(OmActor* const self,
                   OmInitHandler initial_trans, 
                   OmActorAttr* actor_attr,
                   OmTraceAttr* trace_attr );


/// @brief Starts the actor thread of execution
/// @param self 
void om_actor_start(OmActor* self);

/// @brief Exits actor HSM and stops the thread of execution
/// @param self 
void om_actor_stop(OmActor* self);

/// @brief Sends an event message to an actor
/// @param self 
/// @param message 
void om_actor_message(OmActor* self, OmEvent * message);

/// @brief Shared dispatch code to be used internally by various ports
/// @param self 
/// @param event 
void om_actor_dispatch_(OmActor* self, OmEvent* event);

/// @brief Shared internal function for sending stop request to an Actor
/// @param self 
void om_actor_send_stop_msg_(OmActor* self);

#endif// OM_ACTOR_H_