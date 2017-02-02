/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Hello World example for RISC-V. This is a playground for implementing application-defined memory DUE handlers.
 */ 

#include <stdio.h>
#include "memory_due.h" 
volatile float x[10000000];
volatile float y[10000000];

dueinfo_t mydue;
void* maddr = NULL;
void* baddr = NULL;
void* xaddr_s = NULL;
void* xaddr_e = NULL;
void* yaddr_s = NULL;
void* yaddr_e = NULL;
void* iaddr = NULL;

int test = 0;
int test2 = 0;
int test3 = 0;
int test4 = 0;
int test5 = 0;
int test6 = 0;

void* mystart = NULL;
void* myend = NULL;

void my_due_handler(dueinfo_t *recovery_context);

int main(int argc, char** argv) {
    dump_dueinfo(&mydue);
    printf("&x: %p\n", x);
    printf("&x+10000000: %p\n", x+10000000);
    printf("&y: %p\n", y);
    printf("&y+10000000: %p\n", y+10000000);
    printf("func_start: %p\n", &&func_start);
    printf("func_end: %p\n", &&func_end);
    mystart = &&func_start;
    myend = &&func_end;
    register_user_memory_due_trap_handler(&my_due_handler);
    float m = 2;
    float b = 0;
    long i = 0;
    maddr = (void*)(&m);
    baddr = (void*)(&b);
    xaddr_s = (void*)(x);
    xaddr_e = (void*)(x+10000000);
    yaddr_s = (void*)(y);
    yaddr_e = (void*)(y+10000000);
    iaddr = (void*)(&i);
    func_start:;
    for (i = 0; i < 10000000; i++) {
        x[i] = i;
    }
    for (i = 0; i < 10000000; i++) {
        y[i] = m*x[i]+b;
    }
    func_end:;
    printf("Hello World!\n");
    dump_dueinfo(&mydue);
    printf("test = %d\n", test);
    printf("test2 = %d\n", test2);
    printf("test3 = %d\n", test3);
    printf("test4 = %d\n", test4);
    printf("test5 = %d\n", test5);
    printf("test6 = %d\n", test6);
    printf("func_start: %p\n", &&func_start);
    printf("func_end: %p\n", &&func_end);
    return 0;
}

void my_due_handler(dueinfo_t *recovery_context) {
    mydue.tf = recovery_context->tf;
    mydue.error_in_stack = recovery_context->error_in_stack;
    mydue.error_in_text = recovery_context->error_in_text;
    mydue.error_in_data = recovery_context->error_in_data;
    mydue.error_in_sdata = recovery_context->error_in_sdata;
    mydue.error_in_bss = recovery_context->error_in_bss;
    mydue.error_in_heap = recovery_context->error_in_heap;

    if (mydue.tf.epc >= mystart && mydue.tf.epc < myend)
        test = 1;
    if (mydue.tf.badvaddr >= xaddr_s && mydue.tf.badvaddr < xaddr_e)
        test2 = 1;
    if (mydue.tf.badvaddr >= yaddr_s && mydue.tf.badvaddr < yaddr_e)
        test3 = 1;
    if (mydue.tf.badvaddr == maddr)
        test4 = 1;
    if (mydue.tf.badvaddr == baddr)
        test5 = 1;
    if (mydue.tf.badvaddr == iaddr)
        test6 = 1;
}
    
