#ifndef OM_CONSOLE_SERVICE_H
#define OM_CONSOLE_SERVICE_H

#include <stddef.h>
#include <stdbool.h>
#include "om.h"
#include "om_uart.h"
#include "om_config.h"

/// @file om_console.h
/// @brief Console service for processing commands received over UART




/// Forward declare the console structure
typedef struct OmConsole OmConsole;

/// Callback type for console command handlers
typedef void (*OmConsoleCallback)(OmConsole *self, const char * command,  const char *args);

/// Command structure for console commands
typedef struct {
    const char *command; // Command string (e.g. "help", "status")
    OmConsoleCallback callback; // Function to call when command is executed
    const char *description; // Description of the command for help text
}OmConsoleCommand;

/// Simple console service that processes commands received over UART
/// The console listens for commands terminated by a newline character and executes the corresponding callback
///
/// A "help" command is built in and lists every command in the table with its description.
/// Help lines are sent one at a time, each after the previous OM_EVT_UART_TX_OK, so the
/// console actor is not blocked while a long help list goes out. Do not add "help" to the
/// command table; it is handled before the table is searched.
///
/// Command callbacks send their output with om_uart_printf(self->uart, ...).
///
/// Example usage:
/// @code
/// OmConsoleCommand commands[] = {
///     {"status", console_status_command, "Show system status"},
/// };
///
/// OmConsole console;
/// om_console_init(&console, &uart, commands, sizeof(commands)/sizeof(commands[0]), true, actor_attr, trace_attr);
/// om_actor_start(&console.base);
///
/// void console_status_command(OmConsole *self, const char *command, const char *args) {
///     om_uart_printf(self->uart, "Uptime: %lu seconds\r\n", uptime);
/// }
/// @endcode

typedef struct OmConsole
{
    OmActor base;
    OmUart* uart;
    bool interactive_mode;
    char cmd_buffer[OM_CONSOLE_CMD_BUFFER_SIZE];
    int cmd_buffer_index;
    size_t command_count;
    OmConsoleCommand* commands;
    bool help_requested;  ///< Set by the built-in help command, triggers the transition to the help state
    size_t help_index;    ///< Next command table entry to send while in the help state
}OmConsole;

/// @brief Initialize the console service
/// @param self Console instance
/// @param uart UART instance to use for communication
/// @param commands Array of console commands   
/// @param command_count Number of commands in the array
/// @param interactive_mode If true, the console will send a prompt after each command and wait for input. If false, it will only process commands when they are received without sending prompts or echoing.
/// @param actor_attr Actor attributes for the console's internal actor (priority, stack size, queue size)
/// @param trace_attr Trace attributes for the console's internal actor (name, trace buffer, trace flags)
void om_console_init(OmConsole* self, 
                     OmUart *uart, 
                     OmConsoleCommand *commands, 
                     size_t command_count,
                     bool interactive_mode, 
                     OmActorAttr *actor_attr,
                     OmTraceAttr *trace_attr);



/// @brief Parse command arguments from a command line
/// @param args Command line arguments string (everything after the command)
/// @param argv Array to store the parsed arguments
/// @param argc Pointer to store the number of parsed arguments
void om_console_parse_args(const char *args, char *argv[], int *argc);

#endif // OM_CONSOLE_SERVICE_H