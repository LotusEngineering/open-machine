#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_
#include "om.h"
#include "brew_control.h"

typedef struct 
{
    OmActor base;
    OmBus* button_bus;
    BrewControl* brew_control;
    TeaType selected_type;
    BrewStatus status;
    OmTimer status_timer;
}UserInterface;




void ui_ctor(UserInterface* self, OmBus* button_bus, BrewControl* brew_control, OmTrace* trace);

#endif //USER_INTERFACE_H_ 
