/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Hello World example for RISC-V. This is a playground for implementing application-defined memory DUE handlers.
 */ 

#include <stdio.h>
#include <string.h>
#include "hello.h"
#include "foo.h"
#include "memory_due.h" 
#include "handlers.h"

double x[ARRAY_SIZE];
double y[ARRAY_SIZE];

int main(int argc, char** argv) {
    double m,b;
    long i;
    int injected_init, injected_compute = 0;

    EN_RECOVERY(main,m,sizeof(double))
    EN_RECOVERY(main,b,sizeof(double))
    EN_RECOVERY(main,i,sizeof(long))
    EN_RECOVERY_PTR(main,x,ARRAY_SIZE*sizeof(double))
    EN_RECOVERY_PTR(main,y,ARRAY_SIZE*sizeof(double))

    BEGIN_DUE_RECOVERY(main, overall, STRICTNESS_STRICT) 
    BEGIN_DUE_RECOVERY(main, init, STRICTNESS_STRICT)
    //Initialization
    m = 15.22;
    b = 57198;
    i = 0;
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (!injected_init && i == 77) {
            injected_init = 1;
            INJECT_DUE_DATA(0,10)
        }
        x[i] = (double)i;
    }
    END_DUE_RECOVERY(main, init)
   
    BEGIN_DUE_RECOVERY(main, compute, STRICTNESS_DEFAULT)
    //Computation
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (!injected_compute && i == 23) {
            injected_compute = 1;
            INJECT_DUE_DATA(0,10)
        }
        foo(y+i, x[i], m, b);
    }
    END_DUE_RECOVERY(main, compute)
   
    printf("Hello World!\n");
    
    //Report DUE information from each region
    dump_dueinfo(&DUE_INFO(main, overall));
    dump_dueinfo(&DUE_INFO(main, init));
    dump_dueinfo(&DUE_INFO(main, compute));
    
    END_DUE_RECOVERY(main, overall)

    return 0;
}
