#ifndef OM_TRACE_SVC_H
#define OM_TRACE_SVC_H
#include "om.h"
#include <stdint.h>


/// @file om_trace.h
/// @brief Trace service for logging or printing trace messages 
/// The trace service is an actor designed to periodically pull trace messages from a trace buffer and log/print them using a provided printf function.
/// Example usage:
/// @code
/// // Define a printf function (could be a wrapper around standard printf or a custom logging function
/// int my_printf(const char* format, ...) {
///     va_list args;
///     va_start(args, format);
///     int result = vprintf(format, args);
///     va_end(args);
///     return result;
/// }
/// // Initialize trace service
/// OmTraceSvc trace_svc;
/// OmTrace trace_buffer[TRACE_LIST_SIZE];
/// om_trace_init(&trace_buffer, TRACE_LIST_SIZE);          
/// om_trace_svc_init(&trace_svc, 
///                   my_printf,    
///                   &trace_buffer,
///                   50, // Pull trace messages every 50 ms
///                   actor_attr,
///                   trace_attr);
/// om_actor_start(&trace_svc.base);
/// @endcode


typedef int (*OmTracePrintf)(const char* format, ...);

typedef struct OmTraceSvc
{
    OmActor base;
    OmTracePrintf printf;
    OmTrace* trace;
    uint32_t trace_rate_msec;
    OmTimer timer;
}OmTraceSvc;

/// @brief Initialize the trace service
/// @param self Trace instance
/// @param printf Printf function to use for logging/tracing messages
/// @param trace Trace instance to use to pull trace messages from
/// @param actor_attr Actor attributes for the trace's internal actor (priority, stack size, queue size)
/// @param trace_attr Trace attributes for the trace's internal actor (name, trace buffer, trace flags)
void om_trace_svc_init(OmTraceSvc* self, 
                       OmTracePrintf printf,
                       OmTrace* trace,
                       uint32_t trace_rate_msec,
                       OmActorAttr* actor_attr,
                       OmTraceAttr* trace_attr);




#endif // OM_TRACE_SVC_H