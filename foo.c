/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "foo.h"
#include "memory_due.h"
#include <stdio.h>

void foo(double* y, double x, double m, double b) {
    volatile double btmp = b;
    for (int i = 0; i < 10; i++)
        *y = m*x+btmp;
}
