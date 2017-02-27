/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Handler definitions for dealing with memory DUEs
 */ 

#include "handlers.h"
#include "memory_due.h"
#include "hello.h"
#include <stdio.h> //sprintf()
#include <string.h> //memcpy()

//Define relevant global data structures that are needed for DUE handlers at runtime
DECL_DUE_INFO(main, overall)
DECL_DUE_INFO(main, init)
DECL_DUE_INFO(main, compute)
DECL_RECOVERY(main,i,unsigned long)
DECL_RECOVERY(main,x,float)
DECL_RECOVERY(main,y,float)

//Define handler functions

int DUE_RECOVERY_HANDLER(main, overall, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    COPY_DUE_INFO(main, overall, recovery_context)
    /***************************************************************************/

    /******************************* INIT **************************************/
    int retval = -1;
    sprintf(recovery_context->expl, "Unknown error scope");
    /***************************************************************************/
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(main, overall, recovery_context)
    return retval;
    /***************************************************************************/
}

int DUE_RECOVERY_HANDLER(main, init, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    COPY_DUE_INFO(main, init, recovery_context)
    /***************************************************************************/

    /******************************* INIT **************************************/
    int retval = -1;
    sprintf(recovery_context->expl, "Unknown error scope");
    /***************************************************************************/
    
    //TODO: Can we write this using a switch-case style using macros that are easy to read?
    //TODO: how to deal with multiple variables per message? For example, two 32-bit ints packed into 64-bit message? Multiple cases below can fire, but we don't want them to.
   
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    /***************************************************************************/

    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    if (DUE_IN(main, init, x)) {
        DUE_IN_SPRINTF(main, init, x, float, recovery_context)
        retval = 1;
    }
    if (DUE_IN(main, init, y)) {
        DUE_IN_SPRINTF(main, init, y, float, recovery_context)
        retval = 1;
    }
    /***************************************************************************/
    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    //If error is in i, it's control flow and we need to be careful. It can still be heuristically recovered but needs bounds check.
    if (DUE_IN(main, init, i)) {
        DUE_IN_SPRINTF(main, init, i, unsigned long, recovery_context)
        unsigned long c_i = 0, smallest = 2*ARRAY_SIZE, smallest_i = 0;
        for (unsigned long i = 0; i < recovery_context->candidates.size; i++) {
            memcpy(&c_i, recovery_context->candidates.candidate_messages[i].bytes, 8); //FIXME: what about mismatched variable and message sizes?
            if (c_i < smallest) {
                smallest = c_i;
                smallest_i = i;
            }
        }
        if (smallest >= 0 && smallest < ARRAY_SIZE) { //Check that this candidate produces a legal i value based on semantics of the code in which it appears. At least one candidate should match or there is a major issue.
            copy_word(&(recovery_context->recovered_message), recovery_context->candidates.candidate_messages+smallest_i);

            //If we choose the smallest valid candidate, then we do not need to restart.
            //g_handler_stack[g_handler_sp].restart = 0;
            //recovery_context->setup.restart = 0;
            retval = 0;
        }
    }
    /***************************************************************************/

    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(main, init, recovery_context)
    return retval;
    /***************************************************************************/
}

int DUE_RECOVERY_HANDLER(main, compute, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    COPY_DUE_INFO(main, compute, recovery_context)
    /***************************************************************************/

    /******************************* INIT **************************************/
    int retval = 1;
    sprintf(recovery_context->expl, "Unknown error scope");
    /***************************************************************************/
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    if (DUE_IN(main, compute, x)) {
        DUE_IN_SPRINTF(main, compute, x, float, recovery_context)
        retval = 1;
    }
    if (DUE_IN(main, compute, y)) {
        DUE_IN_SPRINTF(main, compute, y, float, recovery_context)
        retval = 1;
    }
    if (DUE_IN(main, compute, i)) {
        DUE_IN_SPRINTF(main, compute, i, unsigned long, recovery_context)
        retval = 1;
    }
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(main, compute, recovery_context)
    return retval;
    /***************************************************************************/
}
