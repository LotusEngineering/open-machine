#ifndef BREW_CONTROL_H_
#define BREW_CONTROL_H_
#include "om.h"



typedef enum
{
    TEA_TYPE_BLACK,
    TEA_TYPE_OOLONG,
    TEA_TYPE_GREEN
}TeaType;

typedef struct 
{
    OmActor base;
    OmTimer steep_timer;
    TeaType tea_type;    
    int steeping_time_sec;
    OmActor* client;
}BrewControl;


typedef struct 
{
    TeaType tea_type;
    int percent_complete;
}BrewStatus;


typedef struct 
{
    OmPoolEvent base;
    BrewStatus status;
}BrewStatusEvent;


void brew_control_ctor(BrewControl* self, OmTrace* trace);

// Move to ibrew_control.h ?? ibrew_control_request_new(event, type, client)
void brew_control_send_start_request(BrewControl* self, TeaType tea_type, OmActor* client);

BrewStatusEvent* ibrew_status_event_new(BrewStatus const * const brew_status);

//void ibrew_status_event_parse(OmEvent* event, BrewStatus* brew_status);


#endif //BREW_CONTROL_H_ 
