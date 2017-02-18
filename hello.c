/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Hello World example for RISC-V. This is a playground for implementing application-defined memory DUE handlers.
 */ 

#include <stdio.h>
#include <string.h>
#include "memory_due.h" 
#include "foo.h"

#define ARRAY_SIZE 10000

double x[ARRAY_SIZE];
double y[ARRAY_SIZE];

DECL_DUE_INFO(main, 1)
DECL_DUE_INFO(main, 2)
DECL_RECOVERY_PRIMITIVE(main,m)
DECL_RECOVERY_PRIMITIVE(main,b)
DECL_RECOVERY_PRIMITIVE(main,i)
DECL_RECOVERY_OBJECT(main,x)
DECL_RECOVERY_OBJECT(main,y)

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context);
int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context);

int main(int argc, char** argv) {
    double m,b;
    long i;
    int injected = 0;

    EN_RECOVERY_PRIMITIVE(main,m)
    EN_RECOVERY_PRIMITIVE(main,b)
    EN_RECOVERY_PRIMITIVE(main,i)
    EN_RECOVERY_OBJECT(main,x,ARRAY_SIZE*sizeof(double))
    EN_RECOVERY_OBJECT(main,y,ARRAY_SIZE*sizeof(double))

    BEGIN_DUE_RECOVERY(main, 1, STRICTNESS_STRICT)
    //Initialization
    m = 2;
    b = 0;
    i = 0;
    for (i = 0; i < ARRAY_SIZE; i++) {
        b = (double)i;
        if (!injected && i == 50) {
            injected = 1;
            INJECT_DUE_DATA(0,0)
        }
        m = b;
        x[i] = m;
    }
    END_DUE_RECOVERY(main, 1)
    b = 0; 
    m = 2;
   
    BEGIN_DUE_RECOVERY(main, 2, STRICTNESS_DEFAULT)
    //Computation
    for (i = 0; i < ARRAY_SIZE; i++) {
        foo(y+i, x[i], m, b);
    }
    END_DUE_RECOVERY(main, 2)
   
    //Report DUE information from both nested regions
    dump_dueinfo(&DUE_INFO(main, 1));
    DUE_IN_PRINTF(main, 1, x)
    DUE_IN_PRINTF(main, 1, y)
    DUE_AT_PRINTF(main, 1, m)
    DUE_AT_PRINTF(main, 1, b)
    DUE_AT_PRINTF(main, 1, i)


    dump_dueinfo(&DUE_INFO(main, 2));
    DUE_IN_PRINTF(main, 2, x)
    DUE_IN_PRINTF(main, 2, y)
    DUE_AT_PRINTF(main, 2, m)
    DUE_AT_PRINTF(main, 2, b)
    DUE_AT_PRINTF(main, 2, i)

    printf("Hello World!\n");
    return 0;
}

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context) {
    COPY_DUE_INFO(main, 1, recovery_context) //Important to put this first

    int retval = -1;

    //TODO: how to deal with multiple variables per message? For example, two 32-bit ints packed into 64-bit message?

    //Decide what to do based on where the error is.
    if (DUE_IN(main, 1, x)
     || DUE_IN(main, 1, y)
     || DUE_AT(main, 1, m)
     || DUE_AT(main, 1, b)) {
        //If error is in an approximable variable, heuristically recover it using OS default policy and proceed.
        retval = 1;
    } else if (DUE_AT(main, 1, i)) {
        //If error is in i, it's control flow and we need to be careful.
        due_candidates_t c;
        copy_candidates(&c, &(recovery_context->candidates));

        long c_i;
        retval = 1;
        copy_word(&(recovery_context->recovered_message), &(c.candidate_messages[0]));
        for (unsigned i = 0; i < c.size; i++) {
            word_t w;
            copy_word(&w, c.candidate_messages+i);
            memcpy(&c_i, w.bytes, 8);
            if (c_i > 0 && c_i <= ARRAY_SIZE) { //Check that this candidate produces a legal i value based on semantics of the code in which it appears. At least one candidate should match or there is a major issue.
                copy_word(&(recovery_context->recovered_message), &(w));
                retval = 0;
                break;
            }
        }

        //Restart the init just to be safe
        g_handler_stack[g_handler_sp].restart = 1;
        recovery_context->setup.restart = 1;
        COPY_DUE_INFO(main, 1, recovery_context) //Need to do this again because we updated recovery context
    } else { //Error is in something we haven't defined to handle. Probably best to crash.
        retval = -1;
    }

    return retval;
}

int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context) {
    COPY_DUE_INFO(main, 2, recovery_context)
    /******* User-defined recovery begins here ********/
    g_handler_stack[g_handler_sp].restart = 0;
    recovery_context->setup.restart = 0;

    //Return 0 to indicate successful recovery.
    return 1; //Kick back to default policy. TODO: placeholder for something more substantive
}
