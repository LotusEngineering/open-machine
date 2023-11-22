// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <stdint.h>
#include <string.h>
#include "om_trace.h"


void om_trace_ctor(OmTrace* self, OmTraceLogEntry* buffer, size_t buffer_size)
{
    self->entry_list = buffer;
    self->entry_list_size = buffer_size;
    self->read_index = 0;
    self->write_index = 0;
    self->timestamp_usec = 0;
}

bool om_trace_int(OmTrace* self, char const * const name, int value, int base)
{
    // Convert value to string
    char value_as_string[40]; 
    om_trace_itoa(value, value_as_string, base);

    const char* trace_strings[] = {name, value_as_string}; 

    return om_trace_fields(self, trace_strings, 2);  
}

bool om_trace_string(OmTrace* self, char const * const string)
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
        self->entry_list[self->write_index].timestamp_usec = self->timestamp_usec;

        // Copy string into list
        strncpy(self->entry_list[self->write_index].message, string, OM_TRACE_MAX_MESSAGE_LENGTH);

        // Update next write index
        self->write_index = next_write_index;


        return true;
    }
}

bool om_trace_fields(OmTrace* self, char const * const strings[], size_t num_strings)
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
        self->entry_list[self->write_index].timestamp_usec = self->timestamp_usec;

        size_t space_left = OM_TRACE_MAX_MESSAGE_LENGTH;
        memset(self->entry_list[self->write_index].message, 0x00, OM_TRACE_MAX_MESSAGE_LENGTH);

        // Format message
        for(int idx = 0; idx < num_strings; idx++)
        {
            strncat(self->entry_list[self->write_index].message, strings[idx], space_left);
            space_left -= sizeof(strings[idx]);
            if (idx < (num_strings -1 ))
            {
                strncat(self->entry_list[self->write_index].message, ":", space_left);
                space_left -= 1;
            }
        }

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

void om_trace_set_timestamp(OmTrace* self, uint64_t timestamp_usec)
{
    self->timestamp_usec = timestamp_usec;
}

uint64_t om_trace_get_timestamp(OmTrace* self)
{
    return self->timestamp_usec;
}

void om_trace_tick(OmTrace* self, uint32_t elapsed_usec)
{
    self->timestamp_usec += elapsed_usec;
}


/*
 * Based on The following:
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
*/
char* om_trace_itoa(int value, char* result, int base) 
{
    // check that the base if valid
    if (base < 2 || base > 36) 
    { 
        *result = '\0'; 
        return result; 
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
