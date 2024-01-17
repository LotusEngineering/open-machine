// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "om_assert.h"
#include "om_hsm.h"
#include "om_config.h"
#include "om_actor.h"
#include "om_timer.h"
#include "om_pool.h"

OM_ASSERT_FILE_NAME();

/**
 * @brief Actor Port definition for Lite port (no message queues, no memory pools)
 * 
 */
typedef struct OmActorPort
{
    void* lite;
}OmActorPort;

static OmActorPort dummy;



void om_actor_ctor(OmActor* self, OmInitHandler initial_trans)
{
    om_actor_ctor_trace(self, initial_trans, NULL, NULL, OM_TF_NONE);
}

void om_actor_ctor_trace(OmActor * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags)
{
    // Call base machine trace constructor
    om_hsm_ctor_trace(&self->base, initial_trans, name, trace, flags);

    // Set Actor's trace
    self->trace = trace;

    // No "port" for the lite version, it's just state machines
    self->port = &dummy;
}

void om_actor_start(OmActor* self, int priority, size_t queue_size, uint32_t stack_size)
{
    (void)priority;
    (void)queue_size;
    (void)stack_size;

    // Enter the state machine
    om_hsm_enter(&self->base);
}

void om_actor_stop(OmActor* self)
{
    // Exit the state machine
    om_hsm_exit(&self->base, 0);
}

void om_actor_message(OmActor* self, OmEvent *  message)
{
    OM_ASSERT(self != NULL);
    om_hsm_dispatch(&self->base, message);
}
