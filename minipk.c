/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "minipk.h"
#include <stdio.h>
#include <stdint.h>

//Originally defined in riscv-pk/pk/handlers.c
void dump_tf(trapframe_t* tf)
{
  static const char* regnames[] = {
    "z ", "ra", "sp", "gp", "tp", "t0",  "t1",  "t2",
    "s0", "s1", "a0", "a1", "a2", "a3",  "a4",  "a5",
    "a6", "a7", "s2", "s3", "s4", "s5",  "s6",  "s7",
    "s8", "s9", "sA", "sB", "t3", "t4",  "t5",  "t6"
  };

  tf->gpr[0] = 0;

  for(int i = 0; i < 32; i+=4)
  {
    for(int j = 0; j < 4; j++)
      printf("%s %lx%c",regnames[i+j],tf->gpr[i+j],j < 3 ? ' ' : '\n');
  }
  printf("pc %lx va %lx insn       %x sr %lx\n", tf->epc, tf->badvaddr,
         (uint32_t)tf->insn, tf->status);
}
