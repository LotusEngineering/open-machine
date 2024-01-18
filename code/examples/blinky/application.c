// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "application.h"
#include "blinky.h"
#include "board.h"

// Tracing suppoprt
#define TRACE_LIST_SIZE 12
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace application_trace;

// Blinky instances
Blinky blink_red;
Blinky blink_yellow;
Blinky blink_green;



void application_start(int priority)
{
    // Init tracing
    om_trace_init(&application_trace, trace_buffer, TRACE_LIST_SIZE);

    // Create actors, inject the LED they should blink
    OmActorAttr actor_attr = {.priority = priority, 
                                .queue_size = 16,
                                .stack_size = 128 * 8};
    OmTraceAttr trace_attr = {.name = OM_NAME_OF(blink_red), 
                                .trace = &application_trace, 
                                .flags = OM_TF_TRANS};

    blinky_init(&blink_red, BOARD_LED_RED, 2000, &actor_attr, &trace_attr);
    
    trace_attr.name = OM_NAME_OF(blink_yellow);
    blinky_init(&blink_yellow, BOARD_LED_YELLOW, 1000, &actor_attr, &trace_attr);
    
    trace_attr.name = OM_NAME_OF(blink_green);
    blinky_init(&blink_green, BOARD_LED_GREEN, 500, &actor_attr, &trace_attr);

    // Start actors
    om_actor_start(&blink_red.base);
    om_actor_start(&blink_yellow.base);
    om_actor_start(&blink_green.base);
}



