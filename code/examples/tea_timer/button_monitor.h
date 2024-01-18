// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef BUTTON_MONITOR_H_
#define BUTTON_MONITOR_H_
#include "om.h"

typedef struct 
{
    OmActor base;
    OmTimer scan_timer;
    OmTimer held_timer;
    int debounce_counter;
    OmBus* button_bus;
    
}ButtonMonitor;


void button_monitor_init(ButtonMonitor* self, 
                        OmBus* button_bus,  // Event bus to publish to
                        OmActorAttr* actor_attr, 
                        OmTraceAttr* trace_attr);

#endif //BUTTON_MONITOR_H_ 
