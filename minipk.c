/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "minipk.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
        
const char* g_int_regnames[] = {
  "z ", "ra", "sp", "gp", "tp", "t0",  "t1",  "t2",
  "s0", "s1", "a0", "a1", "a2", "a3",  "a4",  "a5",
  "a6", "a7", "s2", "s3", "s4", "s5",  "s6",  "s7",
  "s8", "s9", "sA", "sB", "t3", "t4",  "t5",  "t6"
};

//RISC-V User Spec 2.0 calling convention
/*const char* g_float_regnames[] = {
  "fs0 ", "fs1 ", "fs2 ", "fs3 ", "fs4 ", "fs5 ",  "fs6 ",  "fs7 ",
  "fs8 ", "fs9 ", "fs10", "fs11", "fs12", "fs13",  "fs14",  "fs15",
  "fv0 ", "fv1 ", "fa0 ", "fa1 ", "fa2 ", "fa3 ",  "fa4 ",  "fa5 ",
  "fa6 ", "fa7 ", "ft0 ", "ft1 ", "ft2 ", "ft3 ",  "ft4 ",  "ft5 "
};*/

//RISC-V User Spec 2.1 calling convention
const char* g_float_regnames[] = {
  "ft0 ", "ft1 ", "ft2 ", "ft3 ", "ft4 ", "ft5 ",  "ft6 ",  "ft7 ",
  "fs0 ", "fs1 ", "fa0 ", "fa1 ", "fa2 ", "fa3 ",  "fa4 ",  "fa5 ",
  "fa6 ", "fa7 ", "fs2 ", "fs3 ", "fs4 ", "fs5 ",  "fs6 ",  "fs7 ",
  "fs8 ", "fs9 ", "fs10", "fs11", "ft8 ", "ft9 ",  "ft10",  "ft11"
};


//Originally defined in riscv-pk/pk/handlers.c, modified slightly here
void dump_tf(trapframe_t* tf)
{
  tf->gpr[0] = 0;

  for(size_t i = 0; i < NUM_GPR; i+=4)
  {
    for(size_t j = 0; j < 4; j++)
      printf("%s %016lx%c",g_int_regnames[i+j],tf->gpr[i+j],j < 3 ? ' ' : '\n');
  }
  printf("pc %016lx va %016lx insn       %08x sr %016lx\n", tf->epc, tf->badvaddr,
         (uint32_t)tf->insn, tf->status);
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_word(word_t* dest, word_t* src) {
   if (dest && src && src->size <= MAX_WORD_SIZE) {
       for (size_t i = 0; i < src->size; i++)
           dest->bytes[i] = src->bytes[i];
       dest->size = src->size;

       return 0;
   }

   return -4;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_cacheline(due_cacheline_t* dest, due_cacheline_t* src) {
    if (dest && src && src->size <= MAX_CACHELINE_WORDS) {
        for (size_t i = 0; i < src->size; i++)
            copy_word(dest->words+i, src->words+i);
        dest->size = src->size;
        dest->blockpos = src->blockpos;

        return 0;
    }

    return -4;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_candidates(due_candidates_t* dest, due_candidates_t* src) {
    if (dest && src && src->size <= MAX_CANDIDATE_MSG) {
        for (size_t i = 0; i < src->size; i++)
            copy_word(dest->candidate_messages+i, src->candidate_messages+i);
        dest->size = src->size;
        
        return 0;
    }

    return -4;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_trapframe(trapframe_t* dest, trapframe_t* src) {
   if (dest && src) {
       for (size_t i = 0; i < NUM_GPR; i++)
           dest->gpr[i] = src->gpr[i];
       dest->status = src->status;
       dest->epc = src->epc;
       dest->badvaddr = src->badvaddr;
       dest->cause = src->cause;
       dest->insn = src->insn;
       return 0;
   } 
   
   return -4;
}

//Originally defined in riscv-pk/pk/handlers.c
int copy_float_trapframe(float_trapframe_t* dest, float_trapframe_t* src) {
   if (dest && src) {
       for (size_t i = 0; i < NUM_FPR; i++)
           dest->fpr[i] = src->fpr[i];
       return 0;
   }

   return -4;
}

//Originally defined in riscv-pk/pk/handlers.c
int load_value_from_message(word_t* recovered_message, word_t* load_value, due_cacheline_t* cl, size_t load_size, int offset) {
    if (!recovered_message || !load_value || !cl)
        return -4;
   
    //Init
    load_value->size = 0;
    int msg_size = (int) recovered_message->size; 
    int load_width = (int) load_size;
    int blockpos = (int) cl->blockpos;
    int clsize = (int) cl->size;
    int offset_in_block = (offset < 0 ? -offset : offset) % msg_size;
    int remain = load_width;
    int transferred = 0;
    int curr_blockpos = blockpos + offset/msg_size + ((offset < 0 && offset_in_block != 0) ? -1 : 0); 

    if (msg_size < 0 || msg_size > MAX_WORD_SIZE || load_width < 0 || load_width > MAX_WORD_SIZE || clsize < 0 || clsize > MAX_CACHELINE_WORDS || blockpos < 0 || blockpos > clsize || curr_blockpos < 0 || curr_blockpos > clsize) //Something went wrong
        return -4;
        
    while (remain > 0) {
        if (curr_blockpos == blockpos)
            memcpy(load_value->bytes+transferred, recovered_message->bytes+offset_in_block, (msg_size-offset_in_block > remain ? remain : msg_size-offset_in_block));
        else
            memcpy(load_value->bytes+transferred, cl->words[curr_blockpos].bytes+offset_in_block, (msg_size-offset_in_block > remain ? remain : msg_size-offset_in_block));
        remain -= (msg_size-offset_in_block > remain ? remain : msg_size-offset_in_block);
        offset_in_block = 0;
        transferred = load_width-remain;
        curr_blockpos++;
    }

    load_value->size = load_size;
    return 0;
}
