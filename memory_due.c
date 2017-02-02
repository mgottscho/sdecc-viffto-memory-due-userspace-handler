/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "memory_due.h"
#include <stdio.h>
#include <stdint.h>

user_trap_handler g_user_trap_fptr = &default_user_memory_due_trap_handler;

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

void dump_dueinfo(dueinfo_t* dueinfo) {
    dump_tf(&(dueinfo->tf));
    printf("error_in_stack = %d\n", dueinfo->error_in_stack);
    printf("error_in_text = %d\n", dueinfo->error_in_text);
    printf("error_in_data = %d\n", dueinfo->error_in_data);
    printf("error_in_sdata = %d\n", dueinfo->error_in_sdata);
    printf("error_in_bss = %d\n", dueinfo->error_in_bss);
    printf("error_in_heap = %d\n", dueinfo->error_in_heap);

    printf("_ftext: %p\n", &_ftext);
    printf("_etext: %p\n", &_etext);
    printf("_fdata: %p\n", &_fdata);
    printf("_edata: %p\n", &_edata);
    printf("_fbss: %p\n", &_fbss);
    printf("_end: %p\n", &_end);
}

void register_user_memory_due_trap_handler(user_trap_handler fptr) {
    g_user_trap_fptr = fptr;
    trap_handler entry_trap_fptr = &memory_due_handler_entry;
    asm volatile("or a0, zero, %0;" //Load default entry trap handler fptr into register a0
                 "li a7, 447;" //Load syscall number 447 (SYS_register_user_memory_due_trap_handler) into register a7
                 "ecall;" //Make RISC-V environment call to register our user-defined trap handler
                 :
                 : "r" (entry_trap_fptr));
}

void default_user_memory_due_trap_handler(dueinfo_t* recovery_context) {
    //Do nothing.
}

void memory_due_handler_entry(trapframe_t* tf) {
    dueinfo_t user_recovery_context;

    //Init
    user_recovery_context.error_in_stack = 0;
    user_recovery_context.error_in_text = 0;
    user_recovery_context.error_in_data = 0;
    user_recovery_context.error_in_sdata = 0;
    user_recovery_context.error_in_bss = 0;
    user_recovery_context.error_in_heap = 0;

    //Copy trap frame
    for (int i = 0; i < 32; i++)
        user_recovery_context.tf.gpr[i] = tf->gpr[i];
    user_recovery_context.tf.status = tf->status;
    user_recovery_context.tf.epc = tf->epc;
    user_recovery_context.tf.badvaddr = tf->badvaddr;
    user_recovery_context.tf.cause = tf->cause;
    user_recovery_context.tf.insn = tf->insn;

    //Determine in which segment the memory DUE occured
    if (tf->badvaddr >= tf->gpr[2]-1024 && tf->badvaddr < tf->gpr[2]+1024) //gpr[2] is sp
        user_recovery_context.error_in_stack = 1;
    if (tf->badvaddr >= (long)(&_ftext) && tf->badvaddr < (long)(&_etext))
        user_recovery_context.error_in_text = 1;
    if (tf->badvaddr >= (long)(&_fdata) && tf->badvaddr < (long)(&_edata))
        user_recovery_context.error_in_data = 1;
    if (tf->badvaddr >= (long)(&_edata) && tf->badvaddr < (long)(&_fbss))
        user_recovery_context.error_in_sdata = 1;
    if (tf->badvaddr >= (long)(&_fbss) && tf->badvaddr < (long)(&_end))
        user_recovery_context.error_in_bss = 1;
    user_recovery_context.error_in_heap = 0; //TODO

    //Call user handler
    g_user_trap_fptr(&user_recovery_context);
}

