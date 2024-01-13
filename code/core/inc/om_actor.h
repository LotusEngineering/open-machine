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

// OS Port specific actor data
typedef struct OmActorPort OmActorPort;

typedef struct 
{
    OmHsm base;
    OmTrace* trace;
    OmActorPort port;
}OmActor;

/// @brief Base class to be used by application code for sending responses
typedef struct 
{
    OmEvent base;
    OmActor* sender;
}OmMessageEvent;

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

/// @brief Constructs an actor without any tracing
/// @param self 
/// @param initial_trans 
void om_actor_ctor(OmActor* self, OmInitHandler initial_trans);

/// @brief Constructs an actor with tracing support
/// @param self 
/// @param initial_trans 
/// @param name 
/// @param trace 
/// @param flags 
void om_actor_ctor_trace(OmActor * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags);

/// @brief Starts the actor thread of execution
/// @param self 
/// @param priority 
/// @param queue_size 
/// @param stack_size 
void om_actor_start(OmActor* self, int priority, size_t queue_size, uint32_t stack_size);

/// @brief Stops the actor thread of execution
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

#endif// OM_ACTOR_H_