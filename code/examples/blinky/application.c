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
    om_trace_ctor(&application_trace, trace_buffer, TRACE_LIST_SIZE);

    // Create actors, inject the LED they should blink
    blinky_ctor(&blink_red, "blink_red", &application_trace, BOARD_LED_RED, 2000);
    blinky_ctor(&blink_yellow,"blink_yellow", &application_trace, BOARD_LED_YELLOW, 1000);
    blinky_ctor(&blink_green, "blink_green", &application_trace, BOARD_LED_GREEN, 500);

    // Start actors
    om_actor_start(&blink_red.base, priority, 16, 128 * 8);
    om_actor_start(&blink_yellow.base, priority, 16, 128 * 8);
    om_actor_start(&blink_green.base, priority, 16, 128 * 8);
}



