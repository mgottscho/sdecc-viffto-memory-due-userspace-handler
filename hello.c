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
    int injected2, injected3 = 0;

    EN_RECOVERY(main,m,sizeof(double))
    EN_RECOVERY(main,b,sizeof(double))
    EN_RECOVERY(main,i,sizeof(long))
    EN_RECOVERY_PTR(main,x,ARRAY_SIZE*sizeof(double))
    EN_RECOVERY_PTR(main,y,ARRAY_SIZE*sizeof(double))

    BEGIN_DUE_RECOVERY(main, bar, STRICTNESS_STRICT) 
    BEGIN_DUE_RECOVERY(main, 2, STRICTNESS_STRICT)
    //Initialization
    m = 2;
    b = 0;
    i = 0;
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (!injected2 && i == 77) {
            injected2 = 1;
            INJECT_DUE_DATA(0,10)
        }
        x[i] = (double)i;
    }
    END_DUE_RECOVERY(main, 2)
   
    BEGIN_DUE_RECOVERY(main, 3, STRICTNESS_DEFAULT)
    //Computation
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (!injected3 && i == 23) {
            injected3 = 1;
            INJECT_DUE_DATA(0,10)
        }
        foo(y+i, x[i], m, b);
    }
    END_DUE_RECOVERY(main, 3)
   
    printf("Hello World!\n");
    END_DUE_RECOVERY(main, bar)
    
    //Report DUE information from each region
    dump_dueinfo(&DUE_INFO(main, bar));
    dump_dueinfo(&DUE_INFO(main, 2));
    dump_dueinfo(&DUE_INFO(main, 3));

    return 0;
}
