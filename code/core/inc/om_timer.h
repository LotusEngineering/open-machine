// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_TIMER_H_
#define OM_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "om_actor.h"
#include "om_event.h"


// Port specific data
typedef struct OmTimerPort OmTimerPort;

/// @brief What type of callback should happen when the timer expires
typedef enum
{
    OM_TIMER_CB_ACTOR,  ///< Timer will post event to actor
    OM_TIMER_CB_HSM, ///< Timer will dispatch event to hsm
}OmTimerCallbackType;

/// @brief Timer callback
typedef union 
{
    OmActor* actor;  
    OmHsm* hsm;
}OmTimerCallback;

/// @brief Timer running state
typedef enum
{
    OM_TS_STOPPED, 
    OM_TS_RUNNING, ///< Timer is counting down
    OM_TS_EXPIRED  ///< One shot timer expired
}OmTimerState;

/// @brief Timer operating mode
typedef enum
{
    OM_TM_ONE_SHOT,
    OM_TM_PERIODIC
}OmTimerMode;

/// @brief Timer Event
typedef struct 
{
    OmEvent base;
    OmTimerCallbackType callback_type;
    OmTimerCallback callback;
    OmTimerState state;
    OmTimerMode mode;
    uint32_t period_msec;
    uint32_t remaining_msec;
}OmTimer;

/// Helper macro for casting timer events
#define OM_TIME_EVENT_CAST(event_name_) ((OmTimer *)event_name_)

/// @brief Initialize a timer event used with actor message callback
/// @param self 
/// @param signal 
/// @param name 
/// @param actor 
void om_timer_init(OmTimer* self, OmSignal signal, const char* name, OmActor* actor);

/// @brief Initialize a timer for use with HSM dispatch callback
/// @param self 
/// @param signal 
/// @param name 
/// @param hsm 
void om_timer_init_hsm(OmTimer* self, OmSignal signal, const char* name, OmHsm * hsm);

/// @brief Starts a timer
/// @param self 
/// @param mode 
/// @param time_msec 
void om_timer_start(OmTimer* self, OmTimerMode mode, uint32_t time_msec);

/// @brief Starts a timer with an additional delay
/// @param self 
/// @param mode 
/// @param time_msec 
/// @param delay_time_msec 
/// @note This is usefull if you want to phase timers that have the same period
void om_timer_start_delayed(OmTimer* self, OmTimerMode mode, uint32_t time_msec, uint32_t delay_time_msec);

/// @brief Timer state
/// @param self 
/// @return true if running
bool om_timer_is_running(OmTimer* self);

/// @brief Stops a timer
/// @param self 
void om_timer_stop(OmTimer* self);

/// @brief Must be called periodically to run timers
/// @param elapsed_msec how often the callback is being called
void om_timer_tick(uint32_t elapsed_msec);


#endif //OM_TIMER_H_