#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "om_trace.h"


void om_trace_ctor(OmTrace* self, OmTraceLogEntry* buffer, size_t buffer_size)
{
    self->entry_list = buffer;
    self->entry_list_size = buffer_size;
    self->read_index = 0;
    self->write_index = 0;
    self->elapsed_time_usec = 0;
}

bool om_trace_write(OmTrace* self, const char *format, ...)
{
    if (self == NULL)
    {
        // Tracing can be ignored for an object by setting its pointer to NULL
        return false;
    }
    size_t next_write_index = self->write_index + 1;

    if (next_write_index >= self->entry_list_size)
    {
        next_write_index = 0;
    }

    // Check for full
    if (next_write_index == self->read_index)
    {
        return false;
    }
    else
    {
        // Set timestamp   
        self->entry_list[self->write_index].timestamp_usec = self->elapsed_time_usec;

        // Format message
        va_list args;
        va_start(args, format);
        vsnprintf(self->entry_list[self->write_index].message, (size_t)OM_TRACE_MAX_MESSAGE_LENGTH, format, args);
        va_end(args);

        // Update next write index
        self->write_index = next_write_index;

        return true;

    }

}


bool om_trace_read(OmTrace* self, OmTraceLogEntry* entry)
{
    if (self == NULL)
    {
        // Tracing can be ignored for an object by setting its pointer to NULL
        return false;
    }

    // Check for Empty
    if (self->write_index == self->read_index)
    {
        return false;
    }

    size_t next_read_index = self->read_index + 1;  
    if(next_read_index >= self->entry_list_size)
    {
        next_read_index = 0;
    }

    // Read data into entry
    entry->timestamp_usec = self->entry_list[self->read_index].timestamp_usec;
    strncpy(entry->message, self->entry_list[self->read_index].message, OM_TRACE_MAX_MESSAGE_LENGTH);

    self->read_index = next_read_index;

    return true;
}

void om_trace_clear(OmTrace* self)
{
    if (self == NULL)
    {
        return;
    }

    self->read_index = 0;
    self->write_index = 0;
}

bool om_trace_is_full(OmTrace* self)
{
    if (self == NULL)
    {
        // Tracing can be ignored for an object by setting its pointer to NULL
        return false;
    }
    size_t next_write_index = self->write_index + 1;

    if (next_write_index >= self->entry_list_size)
    {
        next_write_index = 0;
    }

    // Check for full
    if (next_write_index == self->read_index)
    {
        return true;
    }
    else
    {
        return false;
    }

}

void om_trace_tick(OmTrace* self, uint32_t elapsed_usec)
{
    self->elapsed_time_usec += elapsed_usec;
}