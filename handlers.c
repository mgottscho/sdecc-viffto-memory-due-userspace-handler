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
    unsigned variable_matches = 0;
    /***************************************************************************/

    /******************************* INIT **************************************/
    recovery_context->recovery_mode = 1;
    DEFAULT_DUE_SPRINTF(main, overall, recovery_context)
    /***************************************************************************/
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    if (variable_matches > 1) { //Bail out if multiple variables per message
        recovery_context->recovery_mode = 1;
        MULTIPLE_VARIABLES_DUE_SPRINTF(main, overall, recovery_context)
    }
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(main, overall, recovery_context)
    return recovery_context->recovery_mode;
    /***************************************************************************/
}

int DUE_RECOVERY_HANDLER(main, init, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(main, init, recovery_context)
    unsigned variable_matches = 0;
    /***************************************************************************/

    /******************************* INIT **************************************/
    recovery_context->recovery_mode = 1;
    DEFAULT_DUE_SPRINTF(main, init, recovery_context)
    /***************************************************************************/
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    /***************************************************************************/

    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    if (DUE_IN(main, init, x)) {
        DUE_IN_SPRINTF(main, init, x, float, recovery_context)
        variable_matches++;
        recovery_context->recovery_mode = 1;
    }
    if (DUE_IN(main, init, y)) {
        DUE_IN_SPRINTF(main, init, y, float, recovery_context)
        variable_matches++;
        recovery_context->recovery_mode = 1;
    }
    /***************************************************************************/
    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    //If error is in i, it's control flow and we need to be careful. It can still be heuristically recovered but needs bounds check.
    if (DUE_IN(main, init, i)) {
        DUE_IN_SPRINTF(main, init, i, unsigned long, recovery_context)
        variable_matches++;

        unsigned long c_i = 0, smallest = 2*ARRAY_SIZE, smallest_i = 0;
        word_t load_value;
        for (unsigned long i = 0; i < recovery_context->candidates.size; i++) {
            load_value_from_message(&recovery_context->candidates.candidate_messages[i], &load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
            memcpy(&c_i, &load_value, load_value.size); 
            if (c_i < smallest) {
                smallest = c_i;
                smallest_i = i;
            }
        }
        if (smallest >= 0 && smallest < ARRAY_SIZE) { //Check that this candidate produces a legal i value based on semantics of the code in which it appears. At least one candidate should match or there is a major issue.
            copy_word(&(recovery_context->recovered_message), recovery_context->candidates.candidate_messages+smallest_i);
            recovery_context->recovery_mode = 0;
        }
    }
    /***************************************************************************/

    /********** Ensure state is properly committed before returning ************/
    if (variable_matches > 1) { //Bail out if multiple variables per message
        recovery_context->recovery_mode = 1;
        MULTIPLE_VARIABLES_DUE_SPRINTF(main, init, recovery_context)
    }
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(main, init, recovery_context);
    return recovery_context->recovery_mode;
    /***************************************************************************/
}

int DUE_RECOVERY_HANDLER(main, compute, dueinfo_t *recovery_context) {
    /*********** These must come first for macros to work properly  ************/
    static unsigned invocations = 0;
    invocations++;
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(main, compute, recovery_context)
    unsigned variable_matches = 0;
    /***************************************************************************/

    /******************************* INIT **************************************/
    recovery_context->recovery_mode = 1;
    DEFAULT_DUE_SPRINTF(main, compute, recovery_context)
    /***************************************************************************/
    
    /********************** CORRECTNESS-CRITICAL -- FORCE CRASH ****************/
    /***************************************************************************/

   
    /***** FULLY APPROXIMABLE VARIABLES -- FALL BACK TO OS-GUIDED RECOVERY *****/
    if (DUE_IN(main, compute, x)) {
        DUE_IN_SPRINTF(main, compute, x, float, recovery_context)
        variable_matches++;
        recovery_context->recovery_mode = 1;
    }
    if (DUE_IN(main, compute, y)) {
        DUE_IN_SPRINTF(main, compute, y, float, recovery_context)
        variable_matches++;
        recovery_context->recovery_mode = 1;
    }
    if (DUE_IN(main, compute, i)) {
        DUE_IN_SPRINTF(main, compute, i, unsigned long, recovery_context)
        variable_matches++;
        recovery_context->recovery_mode = 1;
    }
    /***************************************************************************/

    
    /*************** APP-DEFINED CUSTOM RECOVERY FOR SPECIFIC CASES ************/
    /***************************************************************************/


    /********** Ensure state is properly committed before returning ************/
    if (variable_matches > 1) { //Bail out if multiple variables per message
        recovery_context->recovery_mode = 1;
        MULTIPLE_VARIABLES_DUE_SPRINTF(main, compute, recovery_context)
    }
    load_value_from_message(&recovery_context->recovered_message, &recovery_context->recovered_load_value, &recovery_context->cacheline, recovery_context->load_size, recovery_context->load_message_offset);
    COPY_DUE_INFO(main, compute, recovery_context)
    return recovery_context->recovery_mode;
    /***************************************************************************/
}
