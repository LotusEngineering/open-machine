// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "application.h"
#include "button_monitor.h"
#include "user_interface.h"
#include "brew_control.h"
#include "board.h"

// Tracing suppoprt
#define TRACE_LIST_SIZE 12
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace application_trace;

// Event bus instances
OmBus button_bus;

// Actor Instances
ButtonMonitor button_mon;
UserInterface user_interface;
BrewControl brew_control;


void application_start(int priority)
{
    // Init tracing
    om_trace_ctor(&application_trace, trace_buffer, TRACE_LIST_SIZE);

    // Init memory pool
    om_pool_init();

    // Init Event Buses
    om_bus_ctor(&button_bus);

    // Create actors, inject any dependencies
    OmActorAttr actor_attr = {.priority = priority, 
                                .queue_size = 16,
                                .stack_size = 128 * 8};
    OmTraceAttr trace_attr = {.name = OM_NAME_OF(button_mon), 
                                .trace = &application_trace, 
                                .flags = OM_TF_TRANS};
    // Init button monitor
    button_monitor_init(&button_mon, &button_bus, &actor_attr, &trace_attr);
    
    // Init brew control
    actor_attr.priority = priority + 1;
    trace_attr.name = OM_NAME_OF(brew_control);
    brew_control_init(&brew_control, &actor_attr, &trace_attr);

    // Init UI
    actor_attr.priority = priority -1;
    trace_attr.name = OM_NAME_OF(user_interface);
    ui_init(&user_interface, &button_bus, &brew_control.base,  &actor_attr, &trace_attr);


    // Start actors
    om_actor_start(&button_mon.base, priority, 16, 128 * 8);
    om_actor_start(&user_interface.base, priority - 1, 16, 128 * 8);
    om_actor_start(&brew_control.base, priority + 1, 16, 128 * 8);
}



void om_assert_handler(const char *file_name, int line)
{
    OM_TRACE_TWO(&application_trace, "ASSERT!!! File", file_name);
    OM_TRACE_VAL_DEC(&application_trace, line);
    
    board_set_leds(BOARD_LED_ALL_ON);

    while(1)
    {
        continue;
    }
}
