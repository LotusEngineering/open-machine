#include <stddef.h>

#include "om_machine.h"
#include "om_assert.h"

// Set assert file name
OM_ASSERT_SET_FILE_NAME();

// Om Internal events
OM_EVENT(OmInitEvent, OM_EVT_INIT);
OM_EVENT(OmEntryEvent, OM_EVT_ENTER);
OM_EVENT(OmExitEvent, OM_EVT_EXIT);

// Private functions
static inline OmStateResult om_call_state_handler_(OmMachine * const self, OmState* state, OmEvent const * const event);
static inline int om_get_path_to_top_(OmState* start_state, OmState* path[OM_MACHINE_MAX_STATE_DEPTH]);


void om_ctor(OmMachine * const self, OmInitHandler initial_trans)
{
    om_ctor_trace(self, initial_trans, NULL, NULL, OM_TF_NONE);
}

void om_ctor_trace(OmMachine * const self, OmInitHandler initial_trans, const char* name, OmTrace* trace, OmTraceFlags flags)
{
    self->current_state = OM_TOP_STATE;
    self->target_state = OM_TOP_STATE;
    self->is_active = false;
    self->exit_code = 0;

    self->initial_trans = initial_trans;
    self->name = name;
    self->trace = trace;
    self->trace_flags = flags;
}


void om_enter(OmMachine * const self)
{

    OmStateResult result = OM_RES_IGNORED;

    // Call Initial Transition handler
    result = self->initial_trans(self);

    // Initial transition must transition
    OM_ASSERT(result == OM_RES_TRANSITION);

    if(self->trace_flags & OM_TF_ENTER)
        om_trace_write(self->trace, "%s:OM_TOP_STATE:om_enter():TRANS(%s)", self->name, self->target_state->name);

    int target_depth = om_get_path_to_top_(self->target_state, self->dst_path);

    // Skip Top state
    target_depth -= 2;

   
    // Enter all target states
    do
    {
        // Enter target state
        result = om_call_state_handler_(self, self->dst_path[target_depth], &OmEntryEvent); 
        // Entry event must not have side effects to state machine state
        OM_ASSERT( (result == OM_RES_HANDLED) || (result == OM_RES_IGNORED));
        
        target_depth--;
    }while (target_depth >= 0);

    int depth = 0;
    result = OM_RES_IGNORED;
    do
    {
        // Init target state
        result = om_call_state_handler_(self, self->target_state, &OmInitEvent);    

        if (result == OM_RES_TRANSITION)
        {
            // Enter target state
            OmStateResult enter_result = om_call_state_handler_(self, self->target_state, &OmEntryEvent); 

            // Entry event must not have side effects to state machine state
            OM_ASSERT( (enter_result == OM_RES_HANDLED) || (enter_result == OM_RES_IGNORED));
        }

        depth++;
        // States are too far nested
        OM_ASSERT(depth < OM_MACHINE_MAX_STATE_DEPTH);
    } while (result == OM_RES_TRANSITION);


    self->current_state = self->target_state;

    self->is_active = true;
}

void om_exit(OmMachine* const self, int exit_code)
{   
    self->is_active = false;
    self->exit_code = exit_code;
    ///#todo call exit functions?
}

bool om_dispatch(OmMachine * const self, OmEvent const * const event)
{
    // Return early if we haven't been entered 
    if (!self->is_active)
    {
        if(self->trace_flags & OM_TF_IGNORED)
            om_trace_write(self->trace, "%s:om_dispatch:%s:Ignored", self->name, event->name);
        return false;
    }

    // Temporary state pointer used to search for handler
    OmState* search_state = self->current_state;

    // Corrupted data, check for wayward pointers!
    OM_ASSERT(search_state != OM_TOP_STATE);

    // Call the current state handler
    OmStateResult result = om_call_state_handler_(self, search_state, event);
    
    int depth = 1;
    
    do
    {
        switch(result)
        {
            // Send ignored events to the parent state
            case OM_RES_IGNORED:
                // Ignored, try parent state
                search_state = search_state->parent;
                depth++;

                // States are too far nested
                OM_ASSERT(depth < OM_MACHINE_MAX_STATE_DEPTH);

                // If we got to the top state, the event wasn't handled by any state
                if(search_state == OM_TOP_STATE)
                {          
                    if (self->trace_flags & OM_TF_UNHANDLED)      
                        om_trace_write(self->trace, "%s:OM_TOP_STATE:%s:Unhandled", self->name, event->name);

                    result = OM_RES_HANDLED;
                    continue;
                }
            break;

            case OM_RES_HANDLED:
                continue;
            break;

            case OM_RES_EXIT:
                // Exit all the states from the current state
                search_state = self->current_state;
                while (search_state != OM_TOP_STATE)
                {
                    result = om_call_state_handler_(self, search_state, &OmExitEvent);           
                    search_state = search_state->parent;
                }
                self->current_state = OM_TOP_STATE;
                
                // Exit the state machine
                self->is_active = false;

                result = OM_RES_HANDLED;
                continue;        
            break;
            
            case OM_RES_TRANSITION:
            {
                // Build a list of source state parents
                int src_idx = om_get_path_to_top_(self->current_state, self->src_path);
                
                // Build a list of destination state parents
                int dst_idx = om_get_path_to_top_(self->target_state, self->dst_path);         
                
                // Find Least Common Ancestor (LCA)
                while((src_idx != 0) || (dst_idx != 0))
                {
                    if (src_idx > 0)
                    {
                        src_idx--;
                    }
                    if (dst_idx > 0)
                    {
                        dst_idx--;
                    } 
                    if (self->src_path[src_idx] == self->dst_path[dst_idx])
                    {
                        // Keep updating the common ancestor until there is a mismatch
                        search_state = self->src_path[src_idx];
                    }
                    else
                    {
                        // Mismatch, break out of loop
                        break;
                    }
                }
                if (self->current_state == self->target_state)
                {
                    // Transiton directly from current back to current
                    om_call_state_handler_(self, self->current_state, &OmExitEvent); 
                    om_call_state_handler_(self, self->current_state, &OmEntryEvent); 
                }
                else if (self->current_state->parent == self->target_state)
                {
                    // Local Transition from substate to direct parent
                    // ...just exit sub state, parent already entered
                    om_call_state_handler_(self, self->current_state, &OmExitEvent); 
                }
                else if (self->current_state == self->target_state->parent)
                {
                    // Local Transition from parent to direct substate
                    // ...just enter substate, parent already entered
                    om_call_state_handler_(self, self->target_state, &OmEntryEvent); 
                }
                else
                {
                    // Exit out of the source states until LCA
                    do
                    {
                        om_call_state_handler_(self, self->current_state, &OmExitEvent); 
                        self->current_state = self->current_state->parent;          
                    }
                    while(self->current_state != search_state);

                    // If the LCA is also the target state, exit LCA as well
                    if (search_state == self->target_state)
                    {
                        om_call_state_handler_(self, search_state, &OmExitEvent); 
                    }

                    // Enter into destination state
                    do
                    {
                        result = om_call_state_handler_(self, self->dst_path[dst_idx], &OmEntryEvent); 
                        // Entry event must not have side effects to state machine state
                        OM_ASSERT( (result == OM_RES_HANDLED) || (result == OM_RES_IGNORED));

                        self->current_state = self->dst_path[dst_idx];
                        dst_idx--;      
                    }
                    while(self->current_state != self->target_state);
                }

                // Do the intial transitions at the destination state
                result = OM_RES_IGNORED;
                do
                {
                    // Init target state
                    result = om_call_state_handler_(self, self->target_state, &OmInitEvent);    

                    // Enter target state, if there is one supplied
                    if (result == OM_RES_TRANSITION)
                    {
                        om_call_state_handler_(self, self->target_state, &OmEntryEvent); 
                    }
                } while (result == OM_RES_TRANSITION);

                // Finally, update the current state to the target
                self->current_state = self->target_state;

                result = OM_RES_HANDLED;
                continue;
            }
            break; // case OM_RES_TRANSITION

        }// switch (result)
    
        result = om_call_state_handler_(self, search_state, event);

    } while(result != OM_RES_HANDLED);

    return true;
}

int om_get_exit_code(OmMachine* self)
{
    return self->exit_code;
}

/////////////////// Private Functions ///////////////////////////
inline OmStateResult om_call_state_handler_(OmMachine * const self, OmState* state, OmEvent const * const event)
{
    OmStateResult result = state->handler(self, event);

    if(self->trace != NULL) ///and filter?
    {
        switch(result)
        {
            case OM_RES_HANDLED:
                if(self->trace_flags & OM_TF_HANDLED)
                    om_trace_write(self->trace, "%s:%s:%s:Handled", self->name, state->name, event->name);
            break;
            case OM_RES_TRANSITION:
                if(self->trace_flags & OM_TF_TRANS)
                    om_trace_write(self->trace, "%s:%s:%s:TRANS(%s)", self->name, state->name, event->name, self->target_state->name);
            break;
            case OM_RES_IGNORED:
                if(self->trace_flags & OM_TF_IGNORED)
                    om_trace_write(self->trace, "%s:%s:%s:Ignored", self->name, state->name, event->name);
            break;
            case OM_RES_EXIT:
                if(self->trace_flags & OM_TF_EXIT)
                    om_trace_write(self->trace, "%s:%s:%s:EXIT(%d)", self->name, state->name, event->name, self->exit_code);
            break;      
            default:
                OM_ASSERT(false);
            break;      
        }

    }
    return result;
}

inline int om_get_path_to_top_(OmState* start_state, OmState* path[OM_MACHINE_MAX_STATE_DEPTH])
{  
    int idx = 0;                
    OmState* search_state = start_state;
    do
    {
        path[idx] = search_state;
        search_state = search_state->parent;
        idx++;
        OM_ASSERT(idx < OM_MACHINE_MAX_STATE_DEPTH);
    }
    while(search_state != OM_TOP_STATE);

    // Add top state 
    path[idx] = OM_TOP_STATE;
    idx++;

    // Return path length
    return idx;
}

