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

float x[ARRAY_SIZE];
float y[ARRAY_SIZE];

DECL_DUE_INFO(main, 1)
DECL_DUE_INFO(main, 2)
DECL_RECOVERY_PRIMITIVE(main,m)
DECL_RECOVERY_PRIMITIVE(main,b)
DECL_RECOVERY_PRIMITIVE(main,i)
DECL_RECOVERY_OBJECT(main,x)
DECL_RECOVERY_OBJECT(main,y)

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context, word_t* recovered_value);
int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context, word_t* recovered_value);

int main(int argc, char** argv) {
    float m,b;
    int i;
   
    EN_RECOVERY_PRIMITIVE(main,m)
    EN_RECOVERY_PRIMITIVE(main,b)
    EN_RECOVERY_PRIMITIVE(main,i)
    EN_RECOVERY_OBJECT(main,x,ARRAY_SIZE*sizeof(float))
    EN_RECOVERY_OBJECT(main,y,ARRAY_SIZE*sizeof(float))

    BEGIN_DUE_RECOVERY(main, 1, STRICTNESS_STRICT)
    //Initialization
    m = 2;
    b = 0;
    i = 0;
    for (i = 0; i < ARRAY_SIZE; i++) {
        static int injected = 0;
        if (!injected && i == 50) {
            injected = 1;
            INJECT_DUE_DATA(0,5)
        }
        x[i] = (float)(i);
    }
   
    BEGIN_DUE_RECOVERY(main, 2, STRICTNESS_DEFAULT)
    //Computation
    for (i = 0; i < ARRAY_SIZE; i++) {
        foo(y+i, x[i], m, b);
    }
    END_DUE_RECOVERY(main, 2)
    END_DUE_RECOVERY(main, 1)
   
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

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context, word_t* recovered_value) {
    int retval = -1;

    //Decide what to do based on where the error is.
    if (DUE_IN(main, 1, x)
     || DUE_IN(main, 1, y)
     || DUE_AT(main, 1, m)
     || DUE_AT(main, 1, b)) {
        //If error is in an approximable variable, heuristically recover it using OS default policy and proceed.
        retval = 1;
    } else if (DUE_AT(main, 1, i)) {
        //If error is in i, it's control flow and we need to be careful.
        due_candidates_t c = recovery_context->candidates;

        int c_i;
        for (unsigned i = 0; i < c.size; i++) {
            word_t w = c.candidate_messages[i];
            memcpy(&c_i, &w, w.size);
            if (c_i > 0 || c_i <= ARRAY_SIZE) //Check that this candidate produces a legal i value based on semantics of the code in which it appears
                *recovered_value = w;
        }

        //Restart the init just to be safe
        g_handler_stack[g_handler_sp].restart = 1;
        recovery_context->setup.restart = 1;

        retval = 0;
    } else { //Error is in something we haven't defined to handle. Probably best to crash.
        retval = -1;
    }

    COPY_DUE_INFO(main, 1, recovery_context)
    return retval;
}

int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context, word_t* recovered_value) {
    /******* User-defined recovery begins here ********/
    g_handler_stack[g_handler_sp].restart = 0;
    recovery_context->setup.restart = 0;

    //Return 0 to indicate successful recovery.
    COPY_DUE_INFO(main, 2, recovery_context)
    return 1; //Kick back to default policy. TODO: placeholder for something more substantive
}
