#ifndef OM_TIMER_H_
#define OM_TIMER_H_
#include <stdint.h>
#include <stdbool.h>
#include "om_actor.h"
#include "om_event.h"


// Port specific data
typedef struct OmTimerPort OmTimerPort;

typedef struct 
{
    OmActor* actor;
    OmEvent const * timeout_event;
    OmTimerPort* port;
}OmTimer;

typedef enum
{
    OM_TIMER_ONE_SHOT,
    OM_TIMER_PERIODIC
}OmTimerMode;

void om_timer_ctor(OmTimer* self, OmActor* actor, OmTimerMode mode, OmEvent const * timeout_event);

void om_timer_start(OmTimer* self, uint32_t time_ms);

bool om_timer_is_running(OmTimer* self);

void om_timer_stop(OmTimer* self);



#endif //OM_TIMER_H_