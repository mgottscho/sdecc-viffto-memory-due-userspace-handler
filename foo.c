/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "foo.h"
#include "memory_due.h"
#include <stdio.h>

void foo(float* y, float x, float m, float b) {
    volatile float btmp = b;
    for (int i = 0; i < 10; i++)
        *y = m*x+btmp;
}
