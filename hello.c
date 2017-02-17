/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Hello World example for RISC-V. This is a playground for implementing application-defined memory DUE handlers.
 */ 

#include <stdio.h>
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
//        if (i == 50)
            //INJECT_DUE_DATA(0,10)
        x[i] = (float)(i);
    }
   
    BEGIN_DUE_RECOVERY(main, 2, STRICTNESS_DEFAULT)
    //Computation
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (i == 50)
            INJECT_DUE_DATA(0,10)
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
    /******* User-defined recovery begins here ********/
    g_handler_stack[g_handler_sp].restart = 0;
    recovery_context->setup.restart = 0;

    //Return 0 to indicate successful recovery.
    COPY_DUE_INFO(main, 1, recovery_context)
    return 1;
}

int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context, word_t* recovered_value) {
    /******* User-defined recovery begins here ********/
    g_handler_stack[g_handler_sp].restart = 0;
    recovery_context->setup.restart = 0;

    //Return 0 to indicate successful recovery.
    COPY_DUE_INFO(main, 2, recovery_context)
    return 1; //Kick back to default policy. TODO: placeholder for something more substantive
}
