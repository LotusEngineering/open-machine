#ifndef OM_EVENT_H_
#define OM_EVENT_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum OmReservedSignals
{
    OM_EVT_ENTER,  ///< State Entry Action
    OM_EVT_INIT,   ///< Initial transition
    OM_EVT_EXIT,   ///< State Exit Action
    OM_EVT_USER,   ///< Start of user defined event signals
};

typedef uint32_t OmSignal;

typedef enum 
{
    OM_ET_STATIC,
    OM_ET_POOL, ///< Dynamically allocated from a memory pool
    OM_ET_TIME, ///< Event used with timers
}OmEventType;


typedef struct 
{
    OmEventType type;
    OmSignal signal;
    const char* name;
}OmEvent;



/**
 * @brief Helper macro for defining static const events
 * 
 */
#define OM_EVENT(event_name_, signal_) static const OmEvent (event_name_) = {OM_ET_STATIC,  signal_, #signal_}  

#define OM_EVENT_CAST(event_type_, event_name_) ((event_type_)event_name_)


#endif //OM_EVENT_H_