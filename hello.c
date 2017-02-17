/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Hello World example for RISC-V. This is a playground for implementing application-defined memory DUE handlers.
 */ 

#include <stdio.h>
#include "memory_due.h" 

#define ARRAY_SIZE 10000

typedef struct {
    float val1;
    int val2;
} foo_t;

volatile foo_t x[ARRAY_SIZE];
volatile foo_t y[ARRAY_SIZE];

DECL_DUE_INFO(main, 1)
DECL_DUE_INFO(main, 2)
DECL_RECOVERY_PRIMITIVE(main,m)
DECL_RECOVERY_PRIMITIVE(main,b)
DECL_RECOVERY_PRIMITIVE(main,i)
DECL_RECOVERY_PRIMITIVE(main,tmp)
DECL_RECOVERY_PRIMITIVE(main,tmp2)
DECL_RECOVERY_OBJECT(main,x)
DECL_RECOVERY_OBJECT(main,y)

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context);
int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context);

int main(int argc, char** argv) {
    float m,b;
    int i;
    char padding[64]; //To prevent DUE injection on tmp variables from impacting loop variable i for experiments
    int tmp, tmp2;
    char padding2[64]; //To prevent DUE injection on tmp variables from impacting loop variable i for experiments
   
    EN_RECOVERY_PRIMITIVE(main,m)
    EN_RECOVERY_PRIMITIVE(main,b)
    EN_RECOVERY_PRIMITIVE(main,i)
    EN_RECOVERY_PRIMITIVE(main,tmp)
    EN_RECOVERY_PRIMITIVE(main,tmp2)
    EN_RECOVERY_OBJECT(main,x,ARRAY_SIZE*sizeof(foo_t))
    EN_RECOVERY_OBJECT(main,y,ARRAY_SIZE*sizeof(foo_t))

    BEGIN_DUE_RECOVERY(main, 1, STRICTNESS_DEFAULT)
    m = 2;
    b = 0;
    i = 0;
    for (i = 0; i < ARRAY_SIZE; i++) {
        x[i].val1 = (float)(i);
        x[i].val2 = i;
    }
    END_DUE_RECOVERY(main, 1)
   
    dump_dueinfo(&DUE_INFO(main, 1));
    DUE_IN_PRINTF(main, 1, x)
    DUE_IN_PRINTF(main, 1, y)
    DUE_AT_PRINTF(main, 1, m)
    DUE_AT_PRINTF(main, 1, b)
    DUE_AT_PRINTF(main, 1, i)

    BEGIN_DUE_RECOVERY(main, 2, STRICTNESS_STRICT)
    for (i = 0; i < ARRAY_SIZE; i++) {
        tmp = m*x[i].val1;
        if (i == 50)
            INJECT_DUE_DATA(0,0)
        tmp2 = tmp+b;
        y[i].val1 = tmp2;
        //y[i].val1 = m*x[i].val1+b;
        y[i].val2 = m*x[i].val2+b;
    }
    END_DUE_RECOVERY(main, 2)

    dump_dueinfo(&DUE_INFO(main, 2));
    DUE_IN_PRINTF(main, 2, x)
    DUE_IN_PRINTF(main, 2, y)
    DUE_AT_PRINTF(main, 2, m)
    DUE_AT_PRINTF(main, 2, b)
    DUE_AT_PRINTF(main, 2, i)
    DUE_AT_PRINTF(main, 2, tmp)
    DUE_AT_PRINTF(main, 2, tmp2)

    printf("Hello World!\n");
    return 0;
}

int DUE_RECOVERY_HANDLER(main, 1, dueinfo_t *recovery_context) {
    /******* User-defined recovery begins here ********/
    //g_handler.restart = 1;
    //recovery_context.setup.restart = 1;

    //Return 0 to indicate successful recovery.
    COPY_DUE_INFO(main, 1, recovery_context)
    return 0;
}

int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context) {
    /******* User-defined recovery begins here ********/
    //g_handler.restart = 1;
    //recovery_context.setup.restart = 1;

    //Return 0 to indicate successful recovery.
    COPY_DUE_INFO(main, 2, recovery_context)
    return 1;
}
