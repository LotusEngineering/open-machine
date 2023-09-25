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


/*

@startuml

skin rose

title User Interface

state Idle
Idle: enter: All LEDs Off
state Config{
  state Black :enter:Red LED, tea_type = Black
  state Oolong :enter:Yellow LED, tea_type = Oolong
  state Green :enter: Green LED, tea_type = Green
  Black -> Oolong : BUTTON_PRESS
  Oolong --> Green : BUTTON_PRESS
  Green --> Black : BUTTON_PRESS
}
state Brewing{
  Brewing: enter: Start LED timer, Send Start to Brew Control
  Brewing: BREW_STATUS: Update status 
  Brewing: exit: Stop LED timer
}
state Done
Done: enter: Turn all LEDs on

[*] --> Idle: Subscribe to Button Bus
Idle --> Brewing: BUTTON_RELEASE
Brewing --> Done: BREW_COMPLETE
Done --> Idle: BUTTON_RELEASE
state c <<choice>>
Idle -> c: BUTTON_HELD
c --> Black: tea_type == Black
c --> Oolong: tea_type == Oolong
c --> Green: tea_type == Green
Config -> Idle : TIMEOUT

@enduml



*/

void ui_ctor(UserInterface* self, OmBus* button_bus, BrewControl* brew_control, OmTrace* trace);

#endif //USER_INTERFACE_H_ 
