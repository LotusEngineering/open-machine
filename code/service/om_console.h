#ifndef OM_CONSOLE_SERVICE_H
#define OM_CONSOLE_SERVICE_H

#include <stddef.h>
#include <stdbool.h>
#include "om.h"
#include "om_uart.h"
#include "om_config.h"

/// @file om_console.h
/// @brief Console service for processing commands received over UART




/// Forward declaree the console structure
typedef struct OmConsole OmConsole;

typedef void (*OmConsoleCallback)(OmConsole *self, const char *args);

/// Command structure for console commands
typedef struct {
    const char *command; // Command string (e.g. "help", "status")
    OmConsoleCallback callback; // Function to call when command is executed
    const char *description; // Description of the command for help text
}OmConsoleCommand;

/// Simple console service that processes commands received over UART
/// The console listens for commands terminated by a newline character and executes the corresponding callback  
/// Example usage:
/// @code
/// OmConsoleCommand commands[] = {
///     {"help", console_help_command, "Show help message"},
/// };
///
/// OmConsole console;
/// om_console_init(&console, &uart, commands, true, sizeof(commands)/sizeof(commands[0]), actor_attr, trace_attr);
/// om_actor_start(&console.base);
/// 
// void console_help_command(OmConsole *self, const char *args) {
///    om_console_send_str(self, "Available commands:\r\n");
///    for (size_t i = 0; i < self->command_count; i++) {
///        om_console_tx_buf_start(self, "  ");
///        om_console_tx_buf_append(self, self->commands[i].command);
///        om_console_tx_buf_append(self, ": ");
///        om_console_tx_buf_append(self, self->commands[i].description);
///        om_console_tx_buf_send(self, "\r\n");
///    }
///}
/// @endcode

typedef struct OmConsole
{
    OmActor base;
    OmUart* uart;
    bool interactive_mode;
    char cmd_buffer[OM_CONSOLE_CMD_BUFFER_SIZE];
    int cmd_buffer_index;
    char tx_buffer[OM_CONSOLE_TX_BUFFER_SIZE];
    int tx_buffer_index;
    size_t command_count;
    OmConsoleCommand* commands;
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


/// @brief Send a string immediately over UART
/// @param self Console instance
/// @param str String to send
/// This is a helper function for sending strings without buffering, useful for prompts and immediate responses
void om_console_send_str(OmConsole *self, const char *str);

/// @brief Start a new transmission with the given string
/// @param self Console instance
/// @param str String to start the transmission with
void om_console_tx_buf_start(OmConsole *self, const char *str);

/// @brief Append a string to the current transmission
/// @param self Console instance    
/// @param str String to append
void om_console_tx_buf_append(OmConsole *self, const char *str);

/// @brief Append an integer to the current transmission
/// @param self Console instance
/// @param value Integer value to append
void om_console_tx_buf_append_int(OmConsole *self, int value, int base);

/// @brief End the current transmission with the given string and send it
/// @param self Console instance
/// @param str String to end the transmission with
void om_console_tx_buf_send(OmConsole *self, const char *str);

#endif // OM_CONSOLE_SERVICE_H