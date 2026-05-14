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
    self->tx_buffer_index = 0;
    memset(self->cmd_buffer, 0, OM_CONSOLE_CMD_BUFFER_SIZE);
    memset(self->tx_buffer, 0, OM_CONSOLE_TX_BUFFER_SIZE);
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
            om_console_send_str(self, "\r\nWelcome to Open Machine Console!\r\n");
            _om_console_send_prompt(self);
        }
        result = OM_RES_HANDLED;
        break;

    case OM_EVT_UART_RX_DATA:
        OmUartDataEvent const *const uart_data = OM_EVENT_CAST(OmUartDataEvent);
        if ((uart_data->data_size == 3) && (uart_data->data[0] == 0x1B) && (uart_data->data[1] == 0x5B) && (uart_data->data[2] == 0x41))
        {
            // Up arrow, repeat last command
            om_console_send_str(self, self->cmd_buffer);
            self->cmd_buffer_index = strlen(self->cmd_buffer);
        }
        else
        {
            for (size_t i = 0; i < uart_data->data_size; i++)
            {
                if (uart_data->data[i] == '\r' || uart_data->data[i] == '\n')
                {
                    // Process command
                    self->cmd_buffer[self->cmd_buffer_index] = '\0';
                    om_console_send_str(self, "\r\n");
                    _om_console_process_cmd(self, self->cmd_buffer);
                    self->cmd_buffer_index = 0;
                }
                else if (self->interactive_mode && (uart_data->data[i] == '\b' || uart_data->data[i] == 0x7F) ) // Handle backspace
                {
                    // Backspace
                    if (self->cmd_buffer_index > 0)
                    {
                        self->cmd_buffer_index--;
                        om_console_send_str(self, "\b \b");
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
                        om_uart_write(self->uart, &uart_data->data[i], 1);
                    }

                    // Buffer overflow check
                    if (self->cmd_buffer_index >= OM_CONSOLE_CMD_BUFFER_SIZE)
                    {
                        if (self->interactive_mode)
                        {
                            om_console_send_str(self, "\r\nCommand too long\r\n");
                            _om_console_send_prompt(self);
                        }
                        else 
                        {
                            om_console_send_str(self, "NAK,Length\r\n");
                        }
                        self->cmd_buffer_index = 0;
                    }
                }
            }
        }
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


void om_console_send_str(OmConsole *self, const char *str)
{
    om_uart_write(self->uart, (uint8_t *)str, strlen(str));
}

void om_console_tx_buf_start(OmConsole *self, const char *str) {
    snprintf(self->tx_buffer, OM_CONSOLE_TX_BUFFER_SIZE, "%s", str);
    self->tx_buffer_index = strlen(self->tx_buffer);
}

void om_console_tx_buf_append(OmConsole *self, const char *str) {
    size_t len = strlen(str);
    if (self->tx_buffer_index + len < OM_CONSOLE_TX_BUFFER_SIZE) {
        strcat(self->tx_buffer, str);
        self->tx_buffer_index += len;
    }
}

void om_console_tx_buf_append_int(OmConsole *self, int value, int base) {
    char int_buffer[32];
    snprintf(int_buffer, sizeof(int_buffer), (base == 16) ? "%x" : "%d", value);
    om_console_tx_buf_append(self, int_buffer);
}

void om_console_tx_buf_send(OmConsole *self, const char *str) {
    om_console_tx_buf_append(self, str);
    om_uart_write(self->uart, (uint8_t *)self->tx_buffer, self->tx_buffer_index);
    self->tx_buffer_index = 0;
}




//////////////// Internal helper functions ////////////////
static void _om_console_send_prompt(OmConsole *self)
{
    om_uart_write(self->uart, (uint8_t *)"> ", 2);
}


void _om_console_process_cmd(OmConsole *self, const char *commandLine)
{
    char command[OM_CONSOLE_TX_BUFFER_SIZE];

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
        strncpy(command, commandLine, OM_CONSOLE_TX_BUFFER_SIZE);
        // No arguments, set args to empty string
        args = "";
    }

    // Find and execute the command
    for (size_t i = 0; i < self->command_count; i++) {
        if (strcmp(self->commands[i].command, command) == 0) {
            if (!self->interactive_mode)
            {
                // In non-interactive mode, send ACK for valid command before executing
                om_console_send_str(self, "ACK\r\n");
            }   
            
            self->commands[i].callback(self, args);
            
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
        om_console_tx_buf_start(self, "Unknown command: ");
        om_console_tx_buf_append(self, command);
        om_console_tx_buf_send(self, "\r\n");
        _om_console_send_prompt(self);

    }
    else 
    {
        // Command not found, send NAK message
        om_console_tx_buf_start(self, "NAK,Unknown command: ");
        om_console_tx_buf_append(self, command);
        om_console_tx_buf_send(self, "\r\n");    
    }
}





