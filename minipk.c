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
      printf("%s %016lx%c",regnames[i+j],tf->gpr[i+j],j < 3 ? ' ' : '\n');
  }
  printf("pc %016lx va %016lx insn       %08x sr %016lx\n", tf->epc, tf->badvaddr,
         (uint32_t)tf->insn, tf->status);
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_word(word_t* dest, word_t* src) {
   if (dest && src) {
       for (int i = 0; i < 32; i++)
           dest->bytes[i] = src->bytes[i];
       dest->size = src->size;

       return 0;
   }

   return 1;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_cacheline(due_cacheline_t* dest, due_cacheline_t* src) {
    if (dest && src) {
        for (int i = 0; i < 32; i++)
            copy_word(dest->words+i, src->words+i);
        dest->size = src->size;
        dest->blockpos = src->blockpos;

        return 0;
    }

    return 1;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_candidates(due_candidates_t* dest, due_candidates_t* src) {
    if (dest && src) {
        for (int i = 0; i < 64; i++)
            copy_word(dest->candidate_messages+i, src->candidate_messages+i);
        dest->size = src->size;
        
        return 0;
    }

    return 1;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_trapframe(trapframe_t* dest, trapframe_t* src) {
   if (dest && src) {
       for (int i = 0; i < 32; i++)
           dest->gpr[i] = src->gpr[i];
       dest->status = src->status;
       dest->epc = src->epc;
       dest->badvaddr = src->badvaddr;
       dest->cause = src->cause;
       dest->insn = src->insn;
       return 0;
   } 
   
   return 1;
}
