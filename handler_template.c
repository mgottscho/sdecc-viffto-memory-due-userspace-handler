/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Handler template code for dealing with DUEs.
 * DO NOT compile or include this file directly!
 */ 

#include <memory_due.h>
#include "handler_template.h"

int DUE_RECOVERY_HANDLER(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, recovery_context)
    unsigned variable_matches = 0;
    /***************************************************************************/

    /******************************* INIT **************************************/
    recovery_context->recovery_mode = -1;
    DEFAULT_DUE_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, recovery_context)
    /***************************************************************************/
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    if (DUE_IN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CRITICAL_VARIABLE)) {
        DUE_IN_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CRITICAL_VARIABLE, recovery_context->expl)
        variable_matches++;
        recovery_context->recovery_mode = -1;
    }
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    if (DUE_IN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_APPROXIMABLE_VARIABLE)) {
        DUE_IN_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_APPROXIMABLE_VARIABLE, recovery_context->expl)
        variable_matches++;
        recovery_context->recovery_mode = 1;
    }
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    if (DUE_IN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CUSTOM_VARIABLE)) {
        DUE_IN_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CUSTOM_VARIABLE, recovery_context->expl)
        variable_matches++;
        
        SOME_TYPE candidate_YOUR_CUSTOM_VARIABLE;

        //Iterate over candidates
        for (unsigned i = 0; i < recovery_context->candidates.size; i++) {

            unsigned legal = 0;
            //Check legality of candidate for variable here, based on your own program logic
            if (legal) {
                copy_word(&(recovery_context->recovered_message), recovery_context->candidates.candidate_messages+i);
                
                //Optional: restart DUE region once it reaches the end of its control flow -- be very careful about side-effects and other control-flow possibilities!
                //g_handler_stack[g_handler_sp].restart = 1;
                //recovery_context->setup.restart = 1;

                recovery_context->recovery_mode = 0;
                break;
            }
        }
    }
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    if (variable_matches > 1) { //Bail out if multiple variables per message
        recovery_context->recovery_mode = -1;
        MULTIPLE_VARIABLES_DUE_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, recovery_context)
    }
    if (recovery_context->mem_type == 1) //any instruction DUE
        recovery_context->recovery_mode = 1;
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, recovery_context)
    return recovery_context->recovery_mode;
    /***************************************************************************/
}
