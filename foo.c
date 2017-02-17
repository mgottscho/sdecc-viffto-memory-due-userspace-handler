/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "foo.h"
#include <stdio.h>

void foo(float* y, float x, float m, float b) {
    *y = m*x+b;
}
