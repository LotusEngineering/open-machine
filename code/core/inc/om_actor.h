#ifndef OM_ACTOR_H_
#define OM_ACTOR_H_
#include <stddef.h>
#include "om_machine.h"


// Forward declare actor, actual struct is port specific
//typedef struct OmActor OmActor;

// Forward declare
//struct OmMachine;
struct OmState;
struct OmEvent;

// OS Port specific actor data
typedef struct OmActorPort OmActorPort;

typedef struct 
{
    OmMachine base;
    OmActorPort* port;
}OmActor;

typedef struct 
{
    OmEvent base;
    OmActor* sender;
}OmMessageEvent;


void om_actor_ctor(OmActor* self, OmInitHandler initial_trans);

void om_actor_ctor_trace(OmActor * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags);


void om_actor_start(OmActor* self, int priority, size_t queue_size, uint32_t stack_size);

void om_actor_stop(OmActor* self);

void om_actor_message(OmActor* self, OmEvent  *  message);



#endif// OM_ACTOR_H_