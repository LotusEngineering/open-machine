#include "application.h"
#include "button_monitor.h"
#include "board.h"

// Tracing suppoprt
#define TRACE_LIST_SIZE 12
OmTraceLogEntry trace_buffer[TRACE_LIST_SIZE];
OmTrace application_trace;

// Event bus instances
OmBus button_bus;

// Actor Instances
ButtonMonitor button_mon;



void application_start(int priority)
{
    // Init tracing
    om_trace_ctor(&application_trace, trace_buffer, TRACE_LIST_SIZE);

    // Init Event Buses
    om_bus_ctor(&button_bus);

    // Create actors, inject dependencies
    button_monitor_ctor(&button_mon, &application_trace, &button_bus);

    // Start actors
    om_actor_start(&button_mon.base, priority, 16, 128 * 8);
}



void om_assert_handler(const char *file_name, int line)
{
    om_trace_write(&application_trace, "ASSERT!!! File: %s, Line: %d", file_name, line);
    board_set_led_on(BOARD_LED_RED);
    board_set_led_on(BOARD_LED_GREEN);
    board_set_led_on(BOARD_LED_YELLOW);

    while(1)
    {
        continue;
    }
}
