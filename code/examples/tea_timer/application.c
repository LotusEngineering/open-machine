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
    button_monitor_ctor(&button_mon, &button_bus, &application_trace);
    brew_control_ctor(&brew_control,  &application_trace);
    ui_ctor(&user_interface, &button_bus, &brew_control.base, &application_trace);

    // Start actors
    om_actor_start(&button_mon.base, priority, 16, 128 * 8);
    om_actor_start(&user_interface.base, priority - 1, 16, 128 * 8);
    om_actor_start(&brew_control.base, priority + 1, 16, 128 * 8);
}



void om_assert_handler(const char *file_name, int line)
{
    om_trace_write(&application_trace, "ASSERT!!! File: %s, Line: %d", file_name, line);
    board_set_leds(BOARD_LED_ALL_ON);

    while(1)
    {
        continue;
    }
}
