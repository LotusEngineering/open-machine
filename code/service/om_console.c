#include "om_console.h"
#include <string.h>
#include <stdio.h>

//OM_ASSERT_FILE_NAME();

// Local function prototypes
static void processCommand(OmConsole *self, const char *commandLine);
static void sendPrompt(OmConsole *self);

// Declare Init trans
OmStateResult console_init_trans(OmConsole *self);


// Declare the states
OM_STATE_DECLARE(OmConsole, om_console_super, OM_TOP_STATE);


void om_console_init(OmConsole* self, 
                     OmUart *uart, 
                     OmConsoleCommand *commands, 
                     size_t command_count,
                     OmActorAttr *actor_attr,
                     OmTraceAttr *trace_attr)
{
    // Call base actor trace init
    om_actor_init(&self->base,
                  OM_INIT_CAST(console_init_trans),
                  actor_attr,
                  trace_attr);
                     
    self->uart = uart;
    self->commands = commands;
    self->command_count = command_count;
    self->cmd_buffer_index = 0;
    self->tx_buffer_index = 0;
    memset(self->cmd_buffer, 0, CONSOLE_CMD_BUFFER_SIZE);
    memset(self->tx_buffer, 0, CONSOLE_TX_BUFFER_SIZE);
}

// Initial transition handler
OmStateResult console_init_trans(OmConsole *self)
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
        result = OM_RES_HANDLED;
        break;

    case OM_EVT_UART_RX_DATA:
        OmUartDataEvent const *const uart_data = OM_EVENT_CAST(OmUartDataEvent);
        if ((uart_data->data_size == 3) && (uart_data->data[0] == 0x1B) && (uart_data->data[1] == 0x5B) && (uart_data->data[2] == 0x41))
        {
            // Up arrow, repeat last command
            send_str(self, self->cmd_buffer);
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
                    send_str(self, "\r\n");
                    processCommand(self, self->cmd_buffer);
                    self->cmd_buffer_index = 0;
                }
                else if (uart_data->data[i] == '\b' || uart_data->data[i] == 0x7F) // Handle backspace
                {
                    // Backspace
                    if (self->cmd_buffer_index > 0)
                    {
                        self->cmd_buffer_index--;
                        send_str(self, "\b \b");
                    }
                }
                else
                {
                    // Echo character back to terminal
                    self->cmd_buffer[self->cmd_buffer_index] = (char)uart_data->data[i];
                    om_uart_write(self->uart, &uart_data->data[i], 1);
                    self->cmd_buffer_index++;
                    if (self->cmd_buffer_index >= CONSOLE_CMD_BUFFER_SIZE)
                    {
                        // Buffer overflow
                        send_str(self, "\r\nCommand too long\r\n");
                        sendPrompt(self);
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

void send_str(OmConsole *self, const char *str)
{
    om_uart_write(self->uart, (uint8_t *)str, strlen(str));
}

void tx_buf_start(OmConsole *self, const char *str) {
    snprintf(self->tx_buffer, CONSOLE_TX_BUFFER_SIZE, "%s", str);
    self->tx_buffer_index = strlen(self->tx_buffer);
    om_uart_write(self->uart, (uint8_t *)self->tx_buffer, self->tx_buffer_index);
}

void tx_buf_append(OmConsole *self, const char *str) {
    size_t len = strlen(str);
    if (self->tx_buffer_index + len < CONSOLE_TX_BUFFER_SIZE) {
        strcat(self->tx_buffer, str);
        self->tx_buffer_index += len;
    }
}

void tx_buf_append_int(OmConsole *self, int value, int base) {
    char int_buffer[32];
    snprintf(int_buffer, sizeof(int_buffer), (base == 16) ? "%x" : "%d", value);
    tx_buf_append(self, int_buffer);
}

void tx_buf_send(OmConsole *self, const char *str) {
    tx_buf_append(self, str);
    om_uart_write(self->uart, (uint8_t *)self->tx_buffer, self->tx_buffer_index);
    self->tx_buffer_index = 0;
    memset(self->tx_buffer, 0, CONSOLE_TX_BUFFER_SIZE);
}




//////////////// Internal helper functions ////////////////
static void sendPrompt(OmConsole *self)
{
    om_uart_write(self->uart, (uint8_t *)"> ", 2);
}


void processCommand(OmConsole *self, const char *commandLine)
{
    char command[CONSOLE_TX_BUFFER_SIZE];
    const char *args = strchr(commandLine, ' ');

    // If there are arguments, split the command and args, otherwise the whole line is the command
    if (args)
    {
        size_t commandLength = args - commandLine;
        strncpy(command, commandLine, commandLength);
        command[commandLength] = '\0';
        args++; // Skip the space
    }
    else
    {
        strncpy(command, commandLine, CONSOLE_TX_BUFFER_SIZE);
        args = "";
    }

    // Find and execute the command
    for (size_t i = 0; i < self->command_count; i++) {
        if (strcmp(self->commands[i].command, command) == 0) {
            self->commands[i].callback(self, command);
            sendPrompt(self);
            return;
        }
    }

    // Command not found, send error message
    tx_buf_start(self, "Unknown command: ");
    tx_buf_append(self, command);
    tx_buf_send(self, "\r\n");
    sendPrompt(self);
}

#define MAX_ARGS 10

void parseArgs(const char *args, char *argv[], int *argc)
{
    *argc = 0;
    while (*args && *argc < MAX_ARGS)
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



/////////////
static void om_console_execute_command(OmConsole *self, const char *cmd) {
    for (size_t i = 0; i < self->command_count; i++) {
        if (strcmp(self->commands[i].command, cmd) == 0) {
            self->commands[i].callback(self, cmd);
            return;
        }
    }
    snprintf(self->tx_buffer, CONSOLE_TX_BUFFER_SIZE, "Unknown command: %s\n", cmd);
    om_uart_write(self->uart, (uint8_t *)self->tx_buffer, strlen(self->tx_buffer));
}

void om_console_process_input(OmConsole *self, const char *input) {
    for (size_t i = 0; input[i] != '\0'; i++) {
        char c = input[i];
        if (c == '\n') {
            self->cmd_buffer[self->cmd_buffer_index] = '\0';
            om_console_execute_command(self, self->cmd_buffer);
            self->cmd_buffer_index = 0;
            memset(self->cmd_buffer, 0, CONSOLE_CMD_BUFFER_SIZE);
        } else if (self->cmd_buffer_index < CONSOLE_CMD_BUFFER_SIZE - 1) {
            self->cmd_buffer[self->cmd_buffer_index++] = c;
        }
    }
}