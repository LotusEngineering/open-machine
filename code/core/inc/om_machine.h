#ifndef OM_MACHINE_H_
#define OM_MACHINE_H_

#include <stdint.h>
#include <stdbool.h>

#include "om_trace.h"
#include "om_event.h"



// Forward declare
typedef struct OneMachine_t OmMachine;

typedef enum
{
    OM_RES_IGNORED,    ///< Event was not handled
    OM_RES_HANDLED,    ///< Event was handled
    OM_RES_TRANSITION, ///< Event Resulted in state transition
    OM_RES_EXIT,       ///< Event triggered exiting of state machine
}OmStateResult;

/// @brief Initial Transition Handler Signature
typedef OmStateResult (*OmInitHandler)(OmMachine * const self);


#define OM_INIT_CAST(init_handler_) (OmInitHandler)(init_handler_)


/// @brief State Event Handler Signature
typedef OmStateResult (*OmStateHandler)(OmMachine * const self, OmEvent const * const event);

// Top of state hierarchy, set to parent for top-most states
#define OM_TOP_STATE   NULL

typedef struct OmState
{
    OmStateHandler handler;
    struct OmState* parent;
    const char* name;  
}OmState;


void om_state_ctor(OmState* self, OmState* parent, const char* name);


// Helper macro to concatenate two tokens
#define CONCAT(a, b) a ## b


/**
 * @brief Used to define the handler signature for a state machine
 * 
 * @note: Use without a semi-colon to define your handler
 * 
 */
#define OM_STATE_DEFINE(machine_, state_name_ ) OmStateResult CONCAT(state_name_, _handler)(machine_ * const self, OmEvent const * const event)


/**
 * @brief Used to declare and allocate a state handler
 * 
 */
#define OM_STATE_DECLARE(machine_, state_name_, parent_ptr_)  OM_STATE_DEFINE(machine_, state_name_); \
OmState state_name_ = { (OmStateHandler)CONCAT(state_name_, _handler), parent_ptr_, #state_name_};


/**
 * @brief Exits a state machine and sets state machine inactive 
 * 
 * @param exit_code_ Exit code, available with om_get_exit_code() 
 * @note This is to called in a OmState handler function 
 */
#define OM_EXIT(exit_code_)  (((OmMachine*)self)->exit_code = exit_code_, OM_RES_EXIT)

/**
 * @brief Causes a state transitions
 * 
 * @param new_state_ State to transition to 
 * @note This is to called in a OmState handler function 
 */
#define OM_TRANS(new_state_)  (((OmMachine*)self)->target_state = &new_state_, OM_RES_TRANSITION)


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


typedef struct OneMachine_t
{
    OmState* current_state;
    OmState* target_state;
    bool is_active; ///< State machine has been initialized and not exited
    int exit_code; ///< User defined exit code
    OmState* src_path[OM_MACHINE_MAX_STATE_DEPTH]; 
    OmState* dst_path[OM_MACHINE_MAX_STATE_DEPTH]; 

    OmInitHandler initial_trans;
    const char* name;
    OmTrace* trace;
    OmTraceFlags trace_flags;
}OmMachine;


/// @brief Construct state machine without tracing
/// @param self State machine
/// @param initial_trans Intial transition handler
void om_ctor(OmMachine * const self, OmInitHandler initial_trans);

/// @brief Construct state machine with tracing
/// @param self State machine
/// @param initial_trans Intial transition handler
/// @param name Machine name
/// @param trace Trace suppier
/// @param flags Trace flags
void om_ctor_trace(OmMachine * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags);


/// @brief Enters the state machine via the intial transition provided in the constructor
/// @param self State machine
void om_enter(OmMachine* const self);


void om_exit(OmMachine* const self, int exit_code);

/**
 * @brief Dispatches an event into a state machine
 * 
 * @param self State machine instance
 * @param event The event to dispatch
 * @return true if state machine is active
 * @return false if state machine is inactive (exited or not entered)
 */
bool om_dispatch(OmMachine * const self, OmEvent const * const event);


/**
 * @brief Returns exit code supplied by state unsing OM_EXIT
 * 
 * @param self State machine instance
 */
int om_get_exit_code(OmMachine* self);

#endif // OM_MACHINE_H_