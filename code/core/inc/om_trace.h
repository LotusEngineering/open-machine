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

// Helper Macros for tracing values, assumes module has a trace member
#define OM_TRACE_INT(value_) om_trace_write(self->trace,  #value_ " = %d", value_)
#define OM_TRACE_BOOL(value_) om_trace_write(#value_ " = %s", (value_) ? "true" : "false")



void om_trace_ctor(OmTrace* self, OmTraceLogEntry* buffer, size_t buffer_size);


/**
 * @brief Writes a trace message
 * 
 * @param self 
 * @param format 
 * @param ... 
 * @return true if message was written
 * @return false if message was not written because queue is full
 */
bool om_trace_write(OmTrace* self, const char *format, ...);


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

#endif