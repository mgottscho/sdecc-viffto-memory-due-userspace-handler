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

int set_float_register(size_t frd, unsigned long raw_value) {
    switch (frd) {
        case 0: //f0
            asm volatile("fmv.d.x f0, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 1: //f1
            asm volatile("fmv.d.x f1, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 2: //f2
            asm volatile("fmv.d.x f2, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 3: //f3
            asm volatile("fmv.d.x f3, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 4: //f4
            asm volatile("fmv.d.x f4, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 5: //f5
            asm volatile("fmv.d.x f5, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 6: //f6
            asm volatile("fmv.d.x f6, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 7: //f7
            asm volatile("fmv.d.x f7, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 8: //f8
            asm volatile("fmv.d.x f8, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 9: //f9
            asm volatile("fmv.d.x f9, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 10: //f10
            asm volatile("fmv.d.x f10, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 11: //f11
            asm volatile("fmv.d.x f11, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 12: //f12
            asm volatile("fmv.d.x f12, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 13: //f13
            asm volatile("fmv.d.x f13, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 14: //f14
            asm volatile("fmv.d.x f14, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 15: //f15
            asm volatile("fmv.d.x f15, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 16: //f16
            asm volatile("fmv.d.x f16, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 17: //f17
            asm volatile("fmv.d.x f17, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 18: //f18
            asm volatile("fmv.d.x f18, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 19: //f19
            asm volatile("fmv.d.x f19, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 20: //f20
            asm volatile("fmv.d.x f20, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 21: //f21
            asm volatile("fmv.d.x f21, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 22: //f22
            asm volatile("fmv.d.x f22, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 23: //f23
            asm volatile("fmv.d.x f23, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 24: //f24
            asm volatile("fmv.d.x f24, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 25: //f25
            asm volatile("fmv.d.x f25, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 26: //f26
            asm volatile("fmv.d.x f26, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 27: //f27
            asm volatile("fmv.d.x f27, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 28: //f28
            asm volatile("fmv.d.x f28, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 29: //f29
            asm volatile("fmv.d.x f29, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 30: //f30
            asm volatile("fmv.d.x f30, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        case 31: //f31
            asm volatile("fmv.d.x f31, %0;"
                         :
                         : "r" (raw_value));
            return 0;
        default: //Bad register
            return -4;
    }
}

int get_float_register(size_t frd, unsigned long* raw_value) {
    if (!raw_value)
        return -4;

    unsigned long tmp;
    switch (frd) {
        case 0: //f0
            asm volatile("fmv.x.d %0, f0;"
                         : "=r" (tmp)
                         :);
            break;
        case 1: //f1
            asm volatile("fmv.x.d %0, f1;"
                         : "=r" (tmp)
                         :);
            break;
        case 2: //f2
            asm volatile("fmv.x.d %0, f2;"
                         : "=r" (tmp)
                         :);
            break;
        case 3: //f3
            asm volatile("fmv.x.d %0, f3;"
                         : "=r" (tmp)
                         :);
            break;
        case 4: //f4
            asm volatile("fmv.x.d %0, f4;"
                         : "=r" (tmp)
                         :);
            break;
        case 5: //f5
            asm volatile("fmv.x.d %0, f5;"
                         : "=r" (tmp)
                         :);
            break;
        case 6: //f6
            asm volatile("fmv.x.d %0, f6;"
                         : "=r" (tmp)
                         :);
            break;
        case 7: //f7
            asm volatile("fmv.x.d %0, f7;"
                         : "=r" (tmp)
                         :);
            break;
        case 8: //f8
            asm volatile("fmv.x.d %0, f8;"
                         : "=r" (tmp)
                         :);
            break;
        case 9: //f9
            asm volatile("fmv.x.d %0, f9;"
                         : "=r" (tmp)
                         :);
            break;
        case 10: //f10
            asm volatile("fmv.x.d %0, f10;"
                         : "=r" (tmp)
                         :);
            break;
        case 11: //f11
            asm volatile("fmv.x.d %0, f11;"
                         : "=r" (tmp)
                         :);
            break;
        case 12: //f12
            asm volatile("fmv.x.d %0, f12;"
                         : "=r" (tmp)
                         :);
            break;
        case 13: //f13
            asm volatile("fmv.x.d %0, f13;"
                         : "=r" (tmp)
                         :);
            break;
        case 14: //f14
            asm volatile("fmv.x.d %0, f14;"
                         : "=r" (tmp)
                         :);
            break;
        case 15: //f15
            asm volatile("fmv.x.d %0, f15;"
                         : "=r" (tmp)
                         :);
            break;
        case 16: //f16
            asm volatile("fmv.x.d %0, f16;"
                         : "=r" (tmp)
                         :);
            break;
        case 17: //f17
            asm volatile("fmv.x.d %0, f17;"
                         : "=r" (tmp)
                         :);
            break;
        case 18: //f18
            asm volatile("fmv.x.d %0, f18;"
                         : "=r" (tmp)
                         :);
            break;
        case 19: //f19
            asm volatile("fmv.x.d %0, f19;"
                         : "=r" (tmp)
                         :);
            break;
        case 20: //f20
            asm volatile("fmv.x.d %0, f20;"
                         : "=r" (tmp)
                         :);
            break;
        case 21: //f21
            asm volatile("fmv.x.d %0, f21;"
                         : "=r" (tmp)
                         :);
            break;
        case 22: //f22
            asm volatile("fmv.x.d %0, f22;"
                         : "=r" (tmp)
                         :);
            break;
        case 23: //f23
            asm volatile("fmv.x.d %0, f23;"
                         : "=r" (tmp)
                         :);
            break;
        case 24: //f24
            asm volatile("fmv.x.d %0, f24;"
                         : "=r" (tmp)
                         :);
            break;
        case 25: //f25
            asm volatile("fmv.x.d %0, f25;"
                         : "=r" (tmp)
                         :);
            break;
        case 26: //f26
            asm volatile("fmv.x.d %0, f26;"
                         : "=r" (tmp)
                         :);
            break;
        case 27: //f27
            asm volatile("fmv.x.d %0, f27;"
                         : "=r" (tmp)
                         :);
            break;
        case 28: //f28
            asm volatile("fmv.x.d %0, f28;"
                         : "=r" (tmp)
                         :);
            break;
        case 29: //f29
            asm volatile("fmv.x.d %0, f29;"
                         : "=r" (tmp)
                         :);
            break;
        case 30: //f30
            asm volatile("fmv.x.d %0, f30;"
                         : "=r" (tmp)
                         :);
            break;
        case 31: //f31
            asm volatile("fmv.x.d %0, f31;"
                         : "=r" (tmp)
                         :);
            break;
        default: //Bad register
            return -4;
    }

    *raw_value = tmp;
    return 0;
}
