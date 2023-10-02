// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_TRACE_H_
#define OM_TRACE_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "om_config.h" 


// Structure to represent a log entry
typedef struct 
{
    uint64_t timestamp_usec;
    char message[OM_TRACE_MAX_MESSAGE_LENGTH];
} OmTraceLogEntry;


typedef struct 
{
    OmTraceLogEntry* entry_list;
    size_t entry_list_size;
    size_t write_index;
    size_t read_index;
    uint64_t elapsed_time_usec;
}OmTrace;


void om_trace_ctor(OmTrace* self, OmTraceLogEntry* buffer, size_t buffer_size);


bool om_trace_int(OmTrace* self, char const * const name, int value, int base);


// Helper Macros for tracing values, assumes module has a trace member
#define OM_TRACE_HEX(self_, value_) om_trace_int(self_, #value_, value_, 16);
#define OM_TRACE_DEC(self_, value_) om_trace_int(self_, #value_, value_, 10);

bool om_trace_string(OmTrace* self, char const * const string);


bool om_trace_fields(OmTrace* self, char const * const strings[], size_t num_strings);


#define OM_TRACE_TWO(trace_, str1_, str2_) \
do{ \
    const char* trace_strings[] = {str1_, str2_}; \
    om_trace_fields(trace_, trace_strings, 2);  \
}while(0) 

#define OM_TRACE_FOUR(trace_, str1_, str2_, str3_, str4_) \
do{ \
    const char* trace_strings[] = {str1_, str2_, str3_, str4_}; \
    om_trace_fields(trace_, trace_strings, 4);  \
}while(0) 

#define OM_TRACE_FIVE(trace_, str1_, str2_, str3_, str4_, str5_) \
do{ \
    const char* trace_strings[] = {str1_, str2_, str3_, str4_, str5_}; \
    om_trace_fields(trace_, trace_strings, 5);  \
}while(0) 


/// @brief Reads a trace entery
/// @param self 
/// @param entry 
/// @return true if an entry was read
bool om_trace_read(OmTrace* self, OmTraceLogEntry* entry);


/**
 * @brief Clears trace buffer
 * 
 * @param self 
 */
void om_trace_clear(OmTrace* self);

bool om_trace_is_full(OmTrace* self);

/// @brief Function to be called by application to provide trace timestamp
/// @param elapse_usec 

/// @brief Function to be called by application to provide trace timestamp
/// @param self 
/// @param elapsed_usec How much time since last call (delta time)
void om_trace_tick(OmTrace* self, uint32_t elapsed_usec);

/// @brief Converts an integer to a string
/// @param value 
/// @param result 
/// @param base 
/// @return 
char* om_trace_itoa(int value, char* result, int base);

#endif //OM_TRACE_H_