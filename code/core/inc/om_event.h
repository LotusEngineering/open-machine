#ifndef OM_EVENT_H_
#define OM_EVENT_H_
#include <stdint.h>
#include <stdbool.h>

enum OmReservedSignals
{
    OM_EVT_ENTER,  ///< State Entry Action
    OM_EVT_INIT,   ///< Initial transition
    OM_EVT_EXIT,   ///< State Exit Action
    OM_EVT_USER,   ///< Start of user defined event signals
};

typedef uint32_t OmSignal;

enum OmEventType
{
    OM_ET_STATIC,
    OM_ET_POOL,

    OM_ET_TIME,
};

typedef struct 
{
    OmSignal signal;
    const char* name;

}OmEvent;


/**
 * @brief Helper macro for defining const events
 * 
 */
#define OM_EVT_DEFINE(event_name_) static const OmEvent Event_ ## event_name_ = {EVT_ ## event_name_, "EVT_" #event_name_}  

typedef struct 
{
    OmEvent base;
    uint32_t timeout_us;
    bool repeat;
    bool is_active;
}OmTimeEvent;

#endif //OM_EVENT_H_