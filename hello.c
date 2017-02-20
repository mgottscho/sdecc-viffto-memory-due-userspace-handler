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
DECL_DUE_INFO(main, 3)
DECL_RECOVERY(main,m)
DECL_RECOVERY(main,b)
DECL_RECOVERY(main,i)
DECL_RECOVERY(main,x)
DECL_RECOVERY(main,y)

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context);
int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context);
int DUE_RECOVERY_HANDLER(main, 3, dueinfo_t *recovery_context);

int main(int argc, char** argv) {
    double m,b;
    long i;
    int injected = 0;

    EN_RECOVERY(main,m,sizeof(double))
    EN_RECOVERY(main,b,sizeof(double))
    EN_RECOVERY(main,i,sizeof(long))
    EN_RECOVERY_PTR(main,x,ARRAY_SIZE*sizeof(double))
    EN_RECOVERY_PTR(main,y,ARRAY_SIZE*sizeof(double))

    BEGIN_DUE_RECOVERY(main, 1, STRICTNESS_STRICT) 
    BEGIN_DUE_RECOVERY(main, 2, STRICTNESS_STRICT)
    //Initialization
    m = 2;
    b = 0;
    i = 0;
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (!injected && i == 77) {
            injected = 1;
            INJECT_DUE_DATA(0,10)
        }
        x[i] = (double)i;
    }
    END_DUE_RECOVERY(main, 2)
   
    BEGIN_DUE_RECOVERY(main, 3, STRICTNESS_DEFAULT)
    //Computation
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (i == 25)
            INJECT_DUE_DATA(0,10)
        foo(y+i, x[i], m, b);
    }
    END_DUE_RECOVERY(main, 3)
   
    printf("Hello World!\n");
    END_DUE_RECOVERY(main, 1)
    
    //Report DUE information from each region
    dump_dueinfo(&DUE_INFO(main, 1));
    dump_dueinfo(&DUE_INFO(main, 2));
    dump_dueinfo(&DUE_INFO(main, 3));

    return 0;
}

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context) {
    COPY_DUE_INFO(main, 1, recovery_context) //This must come first
    //TODO: how to make into a switch-case?

    if (DUE_IN(main, 2, x)) {
        DUE_IN_SPRINTF(main, 2, x, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return -1;
    }
    if (DUE_IN(main, 2, y)) {
        DUE_IN_SPRINTF(main, 2, y, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return -1;
    }
    if (DUE_IN(main, 2, m)) {
        DUE_IN_SPRINTF(main, 2, m, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return -1;
    }
    if (DUE_IN(main, 2, b)) {
        DUE_IN_SPRINTF(main, 2, b, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return -1;
    }
    if (DUE_IN(main, 2, i)) {
        DUE_IN_SPRINTF(main, 2, i, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return -1;
    }

    //Error is in something we haven't defined to handle. Probably best to crash.
    sprintf(recovery_context->expl, "Unknown error scope");
    COPY_DUE_INFO(main, 2, recovery_context) //Need to do this again because we updated recovery context
    return -1;
}

int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context) {
    COPY_DUE_INFO(main, 2, recovery_context) //This must come first
    //TODO: how to deal with multiple variables per message? For example, two 32-bit ints packed into 64-bit message?

    //If error is in an approximable variable, heuristically recover it using OS default policy and proceed.
    if (DUE_IN(main, 2, x)) {
        DUE_IN_SPRINTF(main, 2, x, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 2, y)) {
        DUE_IN_SPRINTF(main, 2, y, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 2, m)) {
        DUE_IN_SPRINTF(main, 2, m, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 2, b)) {
        DUE_IN_SPRINTF(main, 2, b, recovery_context->expl)
        COPY_DUE_INFO(main, 2, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 2, i)) {
        //If error is in i, it's control flow and we need to be careful. It can still be heuristically recovered but needs bounds check.
        long c_i;
        for (unsigned i = 0; i < recovery_context->candidates.size; i++) {
            memcpy(&c_i, recovery_context->candidates.candidate_messages[i].bytes, 8); //FIXME: what about mismatched variable and message sizes?
            if (c_i > 0 && c_i <= ARRAY_SIZE) { //Check that this candidate produces a legal i value based on semantics of the code in which it appears. At least one candidate should match or there is a major issue.
                copy_word(&(recovery_context->recovered_message), recovery_context->candidates.candidate_messages+i);
                //Restart section just to be safe to make sure we initialize every iteration of the loop.
                g_handler_stack[g_handler_sp].restart = 1;
                recovery_context->setup.restart = 1;
                DUE_IN_SPRINTF(main, 2, i, recovery_context->expl)
                COPY_DUE_INFO(main, 2, recovery_context) //Need to do this again because we updated recovery context
                return 0;
            }
        }
    }

    //Error is in something we haven't defined to handle. Probably best to crash.
    sprintf(recovery_context->expl, "Unknown error scope");
    COPY_DUE_INFO(main, 2, recovery_context) //Need to do this again because we updated recovery context
    return -1;
}

int DUE_RECOVERY_HANDLER(main, 3, dueinfo_t *recovery_context) {
    COPY_DUE_INFO(main, 3, recovery_context) //This must come first
    //TODO: how to make into a switch-case?

    if (DUE_IN(main, 3, x)) {
        DUE_IN_SPRINTF(main, 3, x, recovery_context->expl)
        COPY_DUE_INFO(main, 3, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 3, y)) {
        DUE_IN_SPRINTF(main, 3, y, recovery_context->expl)
        COPY_DUE_INFO(main, 3, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 3, m)) {
        DUE_IN_SPRINTF(main, 3, m, recovery_context->expl)
        COPY_DUE_INFO(main, 3, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 3, b)) {
        DUE_IN_SPRINTF(main, 3, b, recovery_context->expl)
        COPY_DUE_INFO(main, 3, recovery_context)
        return 1;
    }
    if (DUE_IN(main, 3, i)) {
        DUE_IN_SPRINTF(main, 3, i, recovery_context->expl)
        COPY_DUE_INFO(main, 3, recovery_context)
        return 1;
    }

    //Error is in something we haven't defined to handle. Probably best to crash.
    sprintf(recovery_context->expl, "Unknown error scope");
    COPY_DUE_INFO(main, 3, recovery_context) //Need to do this again because we updated recovery context
    return 1;
}
