#ifndef OM_CONSOLE_SERVICE_H
#define OM_CONSOLE_SERVICE_H

#include <stddef.h>
#include "om.h"
#include "om_uart.h"

/// @file om_console.h
/// @brief Console service for processing commands received over UART


/// Define buffer sizes for console command input and output
#define CONSOLE_CMD_BUFFER_SIZE 80
#define CONSOLE_TX_BUFFER_SIZE 256

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
///     {"help", console_help_command, "Show this help message"},
///     {"status", console_status_command, "Show system status"},
/// };
/// OmConsole console;
/// om_console_init(&console, &uart, commands, sizeof(commands)/sizeof(commands[0]));
/// @endcode
typedef struct OmConsole
{
    OmActor base;
    OmUart* uart;
    char cmd_buffer[CONSOLE_CMD_BUFFER_SIZE];
    int cmd_buffer_index;
    char tx_buffer[CONSOLE_TX_BUFFER_SIZE];
    int tx_buffer_index;
    size_t command_count;
    OmConsoleCommand* commands;
}OmConsole;

/// @brief Initialize the console service
/// @param self Console instance
/// @param uart UART instance to use for communication
/// @param commands Array of console commands   
/// @param command_count Number of commands in the array
void om_console_init(OmConsole* self, 
                     OmUart *uart, 
                     OmConsoleCommand *commands, 
                     size_t command_count,
                     OmActorAttr *actor_attr,
                     OmTraceAttr *trace_attr);

/// @brief Send a string immediately over UART
/// @param self Console instance
/// @param str String to send
/// This is a helper function for sending strings without buffering, useful for prompts and immediate responses
void send_str(OmConsole *self, const char *str);

/// @brief Start a new transmission with the given string
/// @param self Console instance
/// @param str String to start the transmission with
void tx_buf_start(OmConsole *self, const char *str);

/// @brief Append a string to the current transmission
/// @param self Console instance    
/// @param str String to append
void tx_buf_append(OmConsole *self, const char *str);

/// @brief Append an integer to the current transmission
/// @param self Console instance
/// @param value Integer value to append
void tx_buf_append_int(OmConsole *self, int value, int base);

/// @brief End the current transmission with the given string and send it
/// @param self Console instance
/// @param str String to end the transmission with
void tx_buf_send(OmConsole *self, const char *str);

#endif // OM_CONSOLE_SERVICE_H