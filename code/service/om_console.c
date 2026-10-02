#include "om_console.h"
#include "om_uart.h"
#include <string.h>
#include <stdio.h>

//OM_ASSERT_FILE_NAME();



// Local function prototypes
static void _om_console_send_prompt(OmConsole *self);
static void _om_console_process_cmd(OmConsole *self, const char *commandLine);

// Declare Init trans
OmStateResult om_console_init_trans(OmConsole *self);


// Declare the states
OM_STATE_DECLARE(OmConsole, om_console_super, OM_TOP_STATE);
OM_STATE_DECLARE(OmConsole, om_console_idle, &om_console_super);
OM_STATE_DECLARE(OmConsole, om_console_help, &om_console_super);


void om_console_init(OmConsole* self,
                     OmUart *uart,
                     OmConsoleCommand *commands,
                     size_t command_count,
                     bool interactive_mode,
                     OmActorAttr *actor_attr,
                     OmTraceAttr *trace_attr)
{
    // Call base actor trace init
    om_actor_init(&self->base,
                  OM_INIT_CAST(om_console_init_trans),
                  actor_attr,
                  trace_attr);

    self->uart = uart;
    self->commands = commands;
    self->command_count = command_count;
    self->interactive_mode = interactive_mode;
    self->cmd_buffer_index = 0;
    self->help_requested = false;
    self->help_index = 0;
    memset(self->cmd_buffer, 0, OM_CONSOLE_CMD_BUFFER_SIZE);
}

// Initial transition handler
OmStateResult om_console_init_trans(OmConsole *self)
{
    OmStateResult result = OM_TRANS(om_console_super);
    return result;
}

// Super state
OM_STATE_DEFINE(OmConsole, om_console_super)
{
    OmStateResult result = OM_RES_IGNORED;
    switch (event->signal)
    {
    case OM_EVT_ENTER:
        om_uart_attach(self->uart, (OmActor *)self, NULL, NULL, NULL);
        if (self->interactive_mode)
        {
            // Send welcome message
            om_uart_printf(self->uart, "\r\nWelcome to Open Machine Console!\r\n");
            _om_console_send_prompt(self);
        }
        result = OM_RES_HANDLED;
        break;

    case OM_EVT_INIT:
        result = OM_TRANS(om_console_idle);
        break;

    default:
        result = OM_RES_IGNORED;
        break;
    }

    return result;
}

// Idle state, receives and executes commands
OM_STATE_DEFINE(OmConsole, om_console_idle)
{
    OmStateResult result = OM_RES_IGNORED;
    switch (event->signal)
    {
    case OM_EVT_UART_RX_DATA:
        OmUartDataEvent const *const uart_data = OM_EVENT_CAST(OmUartDataEvent);
        if ((uart_data->data_size == 3) && (uart_data->data[0] == 0x1B) && (uart_data->data[1] == 0x5B) && (uart_data->data[2] == 0x41))
        {
            // Up arrow, repeat last command
            om_uart_printf(self->uart, "%s", self->cmd_buffer);
            self->cmd_buffer_index = strlen(self->cmd_buffer);
        }
        else
        {
            for (size_t i = 0; (i < uart_data->data_size) && !self->help_requested; i++)
            {
                if (uart_data->data[i] == '\r' || uart_data->data[i] == '\n')
                {
                    // Process command
                    self->cmd_buffer[self->cmd_buffer_index] = '\0';
                    om_uart_printf(self->uart, "\r\n");
                    _om_console_process_cmd(self, self->cmd_buffer);
                    self->cmd_buffer_index = 0;
                }
                else if (self->interactive_mode && (uart_data->data[i] == '\b' || uart_data->data[i] == 0x7F) ) // Handle backspace
                {
                    // Backspace
                    if (self->cmd_buffer_index > 0)
                    {
                        self->cmd_buffer_index--;
                        om_uart_printf(self->uart, "\b \b");
                    }
                }
                else
                {
                    // Store character in command buffer
                    self->cmd_buffer[self->cmd_buffer_index] = (char)uart_data->data[i];
                    self->cmd_buffer_index++;

                    // Echo character back to terminal
                    if(self->interactive_mode)
                    {
                        om_uart_printf(self->uart, "%c", uart_data->data[i]);
                    }

                    // Buffer overflow check
                    if (self->cmd_buffer_index >= OM_CONSOLE_CMD_BUFFER_SIZE)
                    {
                        if (self->interactive_mode)
                        {
                            om_uart_printf(self->uart, "\r\nCommand too long\r\n");
                            _om_console_send_prompt(self);
                        }
                        else
                        {
                            om_uart_printf(self->uart, "NAK,Length\r\n");
                        }
                        self->cmd_buffer_index = 0;
                    }
                }
            }
        }

        if (self->help_requested)
        {
            // Any input after the help command in this event is dropped
            self->help_requested = false;
            result = OM_TRANS(om_console_help);
        }
        else
        {
            result = OM_RES_HANDLED;
        }
        break;

    default:
        result = OM_RES_IGNORED;
        break;
    }

    return result;
}

// Help state, sends one help line per completed UART write
OM_STATE_DEFINE(OmConsole, om_console_help)
{
    OmStateResult result = OM_RES_IGNORED;
    switch (event->signal)
    {
    case OM_EVT_ENTER:
        self->help_index = 0;
        om_uart_printf(self->uart, self->interactive_mode ? "Available commands:\r\n" : "ACK\r\nAvailable commands:\r\n");
        result = OM_RES_HANDLED;
        break;

    case OM_EVT_UART_TX_OK:
        if (self->help_index < self->command_count)
        {
            OmConsoleCommand const *const cmd = &self->commands[self->help_index];
            om_uart_printf(self->uart, "  %s: %s\r\n", cmd->command, cmd->description);
            self->help_index++;
            result = OM_RES_HANDLED;
        }
        else
        {
            if (self->interactive_mode)
            {
                _om_console_send_prompt(self);
            }
            result = OM_TRANS(om_console_idle);
        }
        break;

    case OM_EVT_UART_RX_DATA:
        // Input received while help is being sent is dropped
        result = OM_RES_HANDLED;
        break;

    default:
        result = OM_RES_IGNORED;
        break;
    }

    return result;
}

void om_console_parse_args(const char *args, char *argv[], int *argc)
{
    *argc = 0;
    while (*args && *argc < OM_CONSOLE_MAX_ARGS)
    {
        while (*args == ' ')
            args++; // Skip leading spaces
        if (*args)
        {
            argv[*argc] = (char *)args;
            (*argc)++;
        }
        while (*args && *args != ' ')
            args++; // Find end of argument
        if (*args)
        {
            *(char *)args = '\0'; // Null-terminate argument
            args++;
        }
    }
}




//////////////// Internal helper functions ////////////////
static void _om_console_send_prompt(OmConsole *self)
{
    om_uart_printf(self->uart, "> ");
}


void _om_console_process_cmd(OmConsole *self, const char *commandLine)
{
    char command[OM_CONSOLE_CMD_BUFFER_SIZE];

    // Try and find space to split command and args
    const char *args = strchr(commandLine, ' ');

    // If there are arguments, split the command and args, otherwise the whole line is the command
    if (args != NULL)
    {
        size_t commandLength = args - commandLine;
        strncpy(command, commandLine, commandLength);
        command[commandLength] = '\0';
        args++; // Skip the space to point at first argument
    }
    else
    {
        strncpy(command, commandLine, OM_CONSOLE_CMD_BUFFER_SIZE);
        // No arguments, set args to empty string
        args = "";
    }

    // Built-in help, sent line by line from the help state (which also sends the ACK and prompt)
    if (strcmp(command, "help") == 0)
    {
        self->help_requested = true;
        return;
    }

    // Find and execute the command
    for (size_t i = 0; i < self->command_count; i++) {
        if (strcmp(self->commands[i].command, command) == 0) {
            if (!self->interactive_mode)
            {
                // In non-interactive mode, send ACK for valid command before executing
                om_uart_printf(self->uart, "ACK\r\n");
            }

            self->commands[i].callback(self, self->commands[i].command, args);

            if(self->interactive_mode)
            {
                // Send prompt after command execution in interactive mode
                _om_console_send_prompt(self);
            }

            return;
        }
    }

    if (self->interactive_mode)
    {
        // Command not found, send error message
        om_uart_printf(self->uart, "Unknown command: %s\r\n", command);
        _om_console_send_prompt(self);
    }
    else
    {
        // Command not found, send NAK message
        om_uart_printf(self->uart, "NAK,Unknown command: %s\r\n", command);
    }
}
