// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef BREW_CONTROL_H_
#define BREW_CONTROL_H_
#include "om.h"
#include "brew_events.h"


typedef struct 
{
    OmActor base;
    OmTimer steep_timer;
    TeaType tea_type;    
    int steeping_time_sec;
    OmActor* client;
}BrewControl;


void brew_control_ctor(BrewControl* self, OmTrace* trace);




#endif //BREW_CONTROL_H_ 
