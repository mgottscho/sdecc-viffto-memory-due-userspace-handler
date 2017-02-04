/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Hello World example for RISC-V. This is a playground for implementing application-defined memory DUE handlers.
 */ 

#include <stdio.h>
#include "memory_due.h" 

#define ARRAY_SIZE 10000000
volatile float x[ARRAY_SIZE];
volatile float y[ARRAY_SIZE];

dueinfo_t mydue;
DECL_RECOVERY(m,main)
DECL_RECOVERY(b,main)
DECL_RECOVERY(i,main)
DECL_RECOVERY_OBJ(x,main)
DECL_RECOVERY_OBJ(y,main)

int test2 = 0;
int test3 = 0;
int test4 = 0;
int test5 = 0;
int test6 = 0;

int my_due_handler(dueinfo_t *recovery_context);

int main(int argc, char** argv) {
    float m,b;
    int i;
    
    EN_RECOVERY(m,main)
    EN_RECOVERY(b,main)
    EN_RECOVERY(i,main)
    EN_RECOVERY_OBJ(x,main,ARRAY_SIZE*sizeof(float))
    EN_RECOVERY_OBJ(y,main,ARRAY_SIZE*sizeof(float))

    BEGIN_DUE_RECOVERY(&my_due_handler)
    m = 2;
    b = 0;
    i = 0;
    for (i = 0; i < 10000000; i++) {
        x[i] = i;
    }
    for (i = 0; i < 10000000; i++) {
        y[i] = m*x[i]+b;
    }
    END_DUE_RECOVERY

    dump_dueinfo(&mydue);
    printf("test2 = %d\n", test2);
    printf("test3 = %d\n", test3);
    printf("test4 = %d\n", test4);
    printf("test5 = %d\n", test5);
    printf("test6 = %d\n", test6);
    
    printf("Hello World!\n");
    return 0;
}

int my_due_handler(dueinfo_t *recovery_context) {
    mydue.tf = recovery_context->tf;
    mydue.valid_tf = recovery_context->valid_tf;
    mydue.error_in_stack = recovery_context->error_in_stack;
    mydue.error_in_text = recovery_context->error_in_text;
    mydue.error_in_data = recovery_context->error_in_data;
    mydue.error_in_sdata = recovery_context->error_in_sdata;
    mydue.error_in_bss = recovery_context->error_in_bss;
    mydue.error_in_heap = recovery_context->error_in_heap;

    void* badvaddr = (void*)(mydue.tf.badvaddr); //Cast for convenience and easier to read

    if (badvaddr >= RECOVERY_ADDR(x,main) && badvaddr < RECOVERY_END_ADDR(x,main))
        test2 = 1;
    if (badvaddr >= RECOVERY_ADDR(y,main) && badvaddr < RECOVERY_END_ADDR(y,main))
        test3 = 1;
    if (badvaddr == RECOVERY_ADDR(m,main))
        test4 = 1;
    if (badvaddr == RECOVERY_ADDR(b,main))
        test5 = 1;
    if (badvaddr == RECOVERY_ADDR(i,main))
        test6 = 1;

    /******* User-defined recovery begins here ********/
    g_restart_due_region = 1;
    /****************/

    return 0;
}
