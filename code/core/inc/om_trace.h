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

#define OM_NAME_OF(name_) #name_

/// @brief Flags used to control what is traced
typedef enum 
{
    OM_TF_NONE = 0x00,
    OM_TF_ENTER = 0x01, ///< Entry of whole machine
    OM_TF_HANDLED = 0x02,
    OM_TF_IGNORED = 0x04,
    OM_TF_TRANS = 0x08,
    OM_TF_EXIT = 0x10,
    OM_TF_UNHANDLED = 0x20,
    OM_TF_ALL = OM_TF_ENTER | OM_TF_HANDLED | OM_TF_IGNORED | OM_TF_TRANS | OM_TF_EXIT | OM_TF_UNHANDLED
}OmTraceFlags;


/// Structure to represent a Trace log entry
typedef struct 
{
    uint64_t timestamp_usec;
    char message[OM_TRACE_MAX_MESSAGE_LENGTH];
} OmTraceLogEntry;

/// @brief Structure to define a trace instance
typedef struct 
{
    OmTraceLogEntry* entry_list;
    size_t entry_list_size;
    size_t write_index;
    size_t read_index;
    uint64_t timestamp_usec;
}OmTrace;

typedef struct 
{
    const char* name; ///< Object name
    OmTrace* trace;  ///< Trace instance
    OmTraceFlags flags; ///< Trace flags
}OmTraceAttr;


/// @brief Constructs a trace instance, typically one per project
/// @param self 
/// @param buffer 
/// @param buffer_size 
void om_trace_init(OmTrace* self, OmTraceLogEntry* buffer, size_t buffer_size);

/// @brief Function to trace an integer
/// @param self 
/// @param name 
/// @param value 
/// @param base 
/// @return 
bool om_trace_int(OmTrace* self, char const * const name, int value, int base);

/// Helper Macro for tracing integer values as decimal
#define OM_TRACE_VAL_DEC(self_, value_) om_trace_int(self_, #value_, value_, 10)

/// Helper Macro for tracing integer values as hexadecimal
#define OM_TRACE_VAL_HEX(self_, value_) om_trace_int(self_, #value_, value_, 16)

/// @brief Function to trace a string
/// @param self 
/// @param string 
/// @return 
bool om_trace_string(OmTrace* self, char const * const string);

/// @brief Used to trace a : separated list of strings
/// @param self 
/// @param strings 
/// @param num_strings 
/// @return 
bool om_trace_fields(OmTrace* self, char const * const strings[], size_t num_strings);

/// Traces two strings str1_:str2_
#define OM_TRACE_TWO(trace_, str1_, str2_) \
do{ \
    const char* trace_strings[] = {str1_, str2_}; \
    om_trace_fields(trace_, trace_strings, 2);  \
}while(0) 

/// Traces a string value 
#define OM_TRACE_VAL_STR(self_, value_) OM_TRACE_TWO(self_, #value_, value_)

/// Traces 3 strings str1_:str2_:str3_
#define OM_TRACE_THREE(trace_, str1_, str2_, str3_) \
do{ \
    const char* trace_strings[] = {str1_, str2_, str3_}; \
    om_trace_fields(trace_, trace_strings, 3);  \
}while(0) 

/// Traces 4 strings str1_:str2_:str3_:str4_
#define OM_TRACE_FOUR(trace_, str1_, str2_, str3_, str4_) \
do{ \
    const char* trace_strings[] = {str1_, str2_, str3_, str4_}; \
    om_trace_fields(trace_, trace_strings, 4);  \
}while(0) 

/// Traces 5 strings str1_:str2_:str3_:str4_:str5_ 
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


/// @brief Clears trace buffer
/// @param self 
void om_trace_clear(OmTrace* self);

/// @brief Trace buffer full detection
/// @param self 
/// @return true if trace buffer is full
bool om_trace_is_full(OmTrace* self);

/// @brief Used to synchronize the timestamp to a value 
/// @param self 
/// @param timestamp_usec 
void om_trace_set_timestamp(OmTrace* self, uint64_t timestamp_usec);

/// @brief Returns the tracetime stamp
/// @param self 
/// @return 
uint64_t om_trace_get_timestamp(OmTrace* self);


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