

states = [
    {
        "name": "disabled",
        "substates": []
    },
    {
        "name": "enabled",
        "substates": ["idle", "active", "error"]
    },
]

def stub_state(file_handle):
    stub_string = """
{
    OmStateResult result = OM_RES_IGNORED;
    switch (event->signal)
    {
//    case OM_EVT_ENTER:
//        result = OM_RES_HANDLED;
//        break;
//    case EVT_TIMEOUT:
//        result = OM_RES_HANDLED;
//        break;
//    case OM_EVT_EXIT:
//        result = OM_RES_HANDLED;
//        break;
    default:
        result = OM_RES_IGNORED;
        break;
    } // end switch
    return result;
}

"""
    file_handle.write(stub_string)

def generate_source(actor_name, states, initial_state):
    filename = actor_name.lower()

    # Generate header file
    with open(f"{filename}.h", "x") as header:
        header.write(f"#ifndef {filename.upper()}_H\n")
        header.write(f"#define {filename.upper()}_H\n\n")
        header.write(f"#include \"om.h\"\n\n")
        
        header.write("typedef struct\n{\n")
        header.write("     OmActor base;\n")
        header.write("     //OmTimer timer;\n}")
        header.write(f"{actor_name};\n\n")

        header.write(f"void {filename}_init({actor_name} * self, " )
        header.write("OmActorAttr* actor_attr, ")
        header.write("OmTraceAttr* trace_attr);\n\n")
                
        header.write("\n#endif\n")

    # Generate source file
    with open(f"{filename}.c", "x") as source:
        source.write(f'#include "om.h"\n')
        source.write(f'#include "{filename}.h"\n\n')
        source.write(f"OM_ASSERT_SET_FILE_NAME(\"{filename}.c\");\n\n")

        # Declare the init trans
        source.write("// Declare Init trans\n")
        source.write(f"OmStateResult {filename}_init_trans({actor_name} *self);\n\n")

        # Declare the states
        source.write("/// Declare the states\n")
        for super_state in states:
            source.write(f"OM_STATE_DECLARE({actor_name}, {filename}_{super_state['name']}, OM_TOP_STATE);\n")
            for sub_state in super_state["substates"]:
                source.write(f"OM_STATE_DECLARE({actor_name}, {filename}_{sub_state}, {filename}_{super_state['name']});\n")  

        # Define the init function
        source.write("\n\n\n/// Define the init function\n")
        source.write(f"void {filename}_init({actor_name} * self, OmActorAttr* actor_attr, OmTraceAttr* trace_attr)\n")
        source.write("{\n     // Self and actor attrs must not be null\n")
        source.write("    OM_ASSERT(self != NULL);\n")
        source.write("    OM_ASSERT(actor_attr != NULL);\n\n")
        source.write("    // Call base actor init\n")
        source.write("    om_actor_init(&self->base,\n")
        source.write(f"                OM_INIT_CAST({filename}_init_trans),\n") 
        source.write("                actor_attr,\n") 
        source.write("                trace_attr);\n")
        source.write("    //om_timer_init(&self->timer, EVT_TIMEOUT, \"TIMEOUT\", &self->base);\n")                                    
        source.write("}\n\n")   


        # Define the intial transition
        source.write("\n\n\n/// Define the initial transition\n")
        source.write(f"OmStateResult {filename}_init_trans({actor_name} *self)\n")
        source.write("{\n")  
        source.write(f"     OmStateResult result = OM_TRANS({filename}_{initial_state});\n")
        source.write("     return result;\n")
        source.write("}\n\n")

        source.write("\n\n\n/// Define the states \n")
        # Define the states
        for super_state in states:
            source.write(f"OM_STATE_DEFINE({actor_name}, {filename}_{super_state['name']})")
            stub_state(source)
            for sub_state in super_state["substates"]:
                source.write(f"OM_STATE_DEFINE({actor_name}, {filename}_{sub_state})")
                stub_state(source)

def query_user_for_states():
    states = []
    while True:
        state_name = input("Enter super state name (or empty to finish): ")
        if len(state_name) == 0:
            break
        
        substates = []
        while True:
            substate_name = input(f"    Enter substate name for '{state_name}' (or empty to finish): ")
            if len(substate_name) == 0:
                break
            substates.append(substate_name)
        
        states.append({
            "name": state_name,
            "substates": substates
        })
    
    return states

# Query user for information
if __name__ == "__main__":
    actor_name = input("Enter actor name: ")
    states = query_user_for_states()
    initial_state = input("Enter initial state: ")
    #print(states)
    generate_source(actor_name, states, initial_state)

