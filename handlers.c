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
DECL_DUE_INFO(main, bar)
DECL_DUE_INFO(main, 2)
DECL_DUE_INFO(main, 3)
DECL_RECOVERY(main,m)
DECL_RECOVERY(main,b)
DECL_RECOVERY(main,i)
DECL_RECOVERY(main,x)
DECL_RECOVERY(main,y)

//Define handler functions

int DUE_RECOVERY_HANDLER(main, bar, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    COPY_DUE_INFO(main, bar, recovery_context)
    /***************************************************************************/

    /******************************* INIT **************************************/
    int retval = -1;
    sprintf(recovery_context->expl, "Unknown error scope");
    /***************************************************************************/
    
    //TODO: Can we write this using a switch-case style using macros that are easy to read?
    //TODO: how to deal with multiple variables per message? For example, two 32-bit ints packed into 64-bit message? Multiple cases below can fire, but we don't want them to.
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    if (DUE_IN(main, bar, x)) {
        DUE_IN_SPRINTF(main, bar, x, recovery_context->expl)
        retval = -1;
    }
    if (DUE_IN(main, bar, y)) {
        DUE_IN_SPRINTF(main, bar, y, recovery_context->expl)
        retval = -1;
    }
    if (DUE_IN(main, bar, m)) {
        DUE_IN_SPRINTF(main, bar, m, recovery_context->expl)
        retval = -1;
    }
    if (DUE_IN(main, bar, b)) {
        DUE_IN_SPRINTF(main, bar, b, recovery_context->expl)
        retval = -1;
    }
    if (DUE_IN(main, bar, i)) {
        DUE_IN_SPRINTF(main, bar, i, recovery_context->expl)
        retval = -1;
    }
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(main, bar, recovery_context)
    return retval;
    /***************************************************************************/
}

int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    COPY_DUE_INFO(main, 2, recovery_context)
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
    if (DUE_IN(main, 2, x)) {
        DUE_IN_SPRINTF(main, 2, x, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 2, y)) {
        DUE_IN_SPRINTF(main, 2, y, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 2, m)) {
        DUE_IN_SPRINTF(main, 2, m, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 2, b)) {
        DUE_IN_SPRINTF(main, 2, b, recovery_context->expl)
        retval = 1;
    }
    /***************************************************************************/
    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    //If error is in i, it's control flow and we need to be careful. It can still be heuristically recovered but needs bounds check.
    if (DUE_IN(main, 2, i)) {
        DUE_IN_SPRINTF(main, 2, i, recovery_context->expl)
        long c_i;
        for (unsigned i = 0; i < recovery_context->candidates.size; i++) {
            memcpy(&c_i, recovery_context->candidates.candidate_messages[i].bytes, 8); //FIXME: what about mismatched variable and message sizes?
            if (c_i > 0 && c_i <= ARRAY_SIZE) { //Check that this candidate produces a legal i value based on semantics of the code in which it appears. At least one candidate should match or there is a major issue.
                copy_word(&(recovery_context->recovered_message), recovery_context->candidates.candidate_messages+i);
                //Restart section just to be safe to make sure we initialize every iteration of the loop.
                g_handler_stack[g_handler_sp].restart = 1;
                recovery_context->setup.restart = 1;
                retval = 0;
                break;
            }
        }
    }
    /***************************************************************************/

    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(main, 2, recovery_context)
    return retval;
    /***************************************************************************/
}

int DUE_RECOVERY_HANDLER(main, 3, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    COPY_DUE_INFO(main, 3, recovery_context)
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
    if (DUE_IN(main, 3, x)) {
        DUE_IN_SPRINTF(main, 3, x, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 3, y)) {
        DUE_IN_SPRINTF(main, 3, y, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 3, m)) {
        DUE_IN_SPRINTF(main, 3, m, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 3, b)) {
        DUE_IN_SPRINTF(main, 3, b, recovery_context->expl)
        retval = 1;
    }
    if (DUE_IN(main, 3, i)) {
        DUE_IN_SPRINTF(main, 3, i, recovery_context->expl)
        retval = 1;
    }
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    COPY_DUE_INFO(main, 3, recovery_context)
    return retval;
    /***************************************************************************/
}
