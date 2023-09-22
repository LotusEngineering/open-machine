#ifndef BREW_CONTROL_H_
#define BREW_CONTROL_H_
#include "om.h"

typedef struct 
{
    OmActor base;
}BrewControl;

void brew_control_ctor(BrewControl* self, OmTrace* trace);

#endif //BREW_CONTROL_H_ 
