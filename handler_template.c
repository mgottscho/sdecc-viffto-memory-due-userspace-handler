/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Handler template code for dealing with DUEs.
 * DO NOT modify, compile, or include this file directly!
 */ 

#include "memory_due.h"

#define YOUR_FUNCTION_NAME foo
#define YOUR_IDENTIFIER bar
#define YOUR_CRITICAL_VARIABLE crit_var
#define YOUR_APPROXIMABLE_VARIABLE approx_var
#define YOUR_CUSTOM_VARIABLE custom_var
#define SOME_TYPE unsigned long 

int DUE_RECOVERY_HANDLER(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    COPY_DUE_INFO(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, recovery_context)
    /***************************************************************************/

    /******************************* INIT **************************************/
    int retval = -1;
    sprintf(recovery_context->expl, "Unknown error scope");
    /***************************************************************************/
    
    //TODO: Can we write this using a switch-case style using macros that are easy to read?
    //TODO: how to deal with multiple variables per message? For example, two 32-bit ints packed into 64-bit message? Multiple cases below can fire, but we don't want them to.
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    if (DUE_IN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CRITICAL_VARIABLE)) {
        DUE_IN_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CRITICAL_VARIABLE, recovery_context->expl)
        retval = -1;
    }
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    if (DUE_IN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_APPROXIMABLE_VARIABLE)) {
        DUE_IN_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_APPROXIMABLE_VARIABLE, recovery_context->expl)
        retval = 1;
    }
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    if (DUE_IN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CUSTOM_VARIABLE)) {
        DUE_IN_SPRINTF(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, YOUR_CUSTOM_VARIABLE, recovery_context->expl)
        
        SOME_TYPE candidate_YOUR_CUSTOM_VARIABLE;

        //Iterate over candidates
        for (unsigned i = 0; i < recovery_context->candidates.size; i++) {
            memcpy(&candidate_YOUR_CUSTOM_VARIABLE, recovery_context->candidates.candidate_messages[i].bytes, sizeof(SOME_TYPE)); //FIXME: what about mismatched variable and message sizes?

            unsigned legal = 0;
            //Check legality of candidate for variable here, based on your own program logic
            if (legal) {
                copy_word(&(recovery_context->recovered_message), recovery_context->candidates.candidate_messages+i);
                
                //Optional: restart DUE region once it reaches the end of its control flow -- be very careful about side-effects and other control-flow possibilities!
                //g_handler_stack[g_handler_sp].restart = 1;
                //recovery_context->setup.restart = 1;

                retval = 0;
                break;
            }
        }
    }
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, recovery_context)
    return retval;
    /***************************************************************************/
}
