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
    OmEvent base;
    bool is_running;
}OmTimeEvent;


typedef struct 
{
    OmActor* actor;
    OmTimeEvent* time_event;
    OmTimerPort* port;
}OmTimer;

typedef enum
{
    OM_TIMER_ONE_SHOT,
    OM_TIMER_PERIODIC
}OmTimerMode;



#define OM_TIME_EVENT(event_name_, signal_) static OmTimeEvent (event_name_) = { {OM_ET_TIME,  signal_, #signal_}, false}  

#define OM_TIME_EVENT_CAST(event_name_) ((OmTimeEvent *)event_name_)

void om_timer_ctor(OmTimer* self, OmActor* actor, OmTimerMode mode, OmTimeEvent* const time_event);

void om_timer_start(OmTimer* self, uint32_t time_ms);

bool om_timer_is_running(OmTimer* self);

void om_timer_stop(OmTimer* self);



#endif //OM_TIMER_H_