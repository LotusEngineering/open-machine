#ifndef OM_H
#define OM_H

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

/// @brief Initial Transition Handler
typedef OmStateResult (*OmInitHandler)(OmMachine * const self);

/// @brief State Event Handler Signature
typedef OmStateResult (*OmStateHandler)(OmMachine * const self, OmEvent const * const event);

// Top of state hierarchy
#define OM_TOP_STATE   NULL

typedef struct OmState
{
    OmStateHandler handler;
    const char* name;  
    struct OmState* parent;
}OmState;

// Helper macro to concatenate two tokens
#define CONCAT(a, b) a ## b


/**
 * @brief Used to define the handler signature for a state machine
 * 
 * @note: Use without a semi-colon to define your handler
 * 
 */
#define OM_STATE_DEFINE(state_name_, machine_) OmStateResult CONCAT(state_name_, _handler)(machine_ * const self, OmEvent const * const event)


/**
 * @brief Used to declare and allocate a state handler
 * 
 */
#define OM_STATE_DECLARE(state_name_, machine_, parent_ptr_)  OM_STATE_DEFINE(state_name_, machine_); \
OmState state_name_ = { (OmStateHandler)CONCAT(state_name_, _handler), #state_name_, parent_ptr_};


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


typedef struct OneMachine_t
{
    OmInitHandler initial_trans;
    OmState* current_state;
    OmState* target_state;
    bool is_active; ///< State machine has been initialized and not exited
    int exit_code; ///< User defined exit code
    OmTrace* trace;
    const char* name;
    OmState* src_path[OM_MACHINE_MAX_STATE_DEPTH]; 
    OmState* dst_path[OM_MACHINE_MAX_STATE_DEPTH]; 
}OmMachine;


/**
 * @brief Used to construct a state machine
 * 
 * @param self State machine instance
 * @param initial_state Initial target stateS
 * @param name State machine name
 * @param trace Pointer to INITIALIZED trace instance
 */
void om_ctor(OmMachine * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace);


/**
 * @brief Enters and initializes machine 
 * @param self 
 */
void om_enter(OmMachine * const self);



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

#endif // OM_H