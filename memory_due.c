/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#include "memory_due.h"
#include "minipk.h"
#include <stdio.h>
#include <stdlib.h>

user_defined_trap_handler g_user_trap_fptr = NULL;
void* g_due_trap_region_pc_start = NULL;
void* g_due_trap_region_pc_end = NULL;
int g_restart_due_region = 0;

void dump_dueinfo(dueinfo_t* dueinfo) {
    if (dueinfo) {
        if (dueinfo->valid_tf == 1) {
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
        } else
            printf("No valid trapframe.\n");
    } else
        printf("No valid DUE info.\n");
}

void register_user_memory_due_trap_handler(user_defined_trap_handler fptr, void* pc_start, void* pc_end) {
    //Save necessary global user state
    g_user_trap_fptr = fptr;
    g_due_trap_region_pc_start = pc_start;
    g_due_trap_region_pc_end = pc_end;

    user_trap_handler entry_trap_fptr = &memory_due_handler_entry;
    asm volatile("or a0, zero, %0;" //Load default entry trap handler fptr into register a0
                 "li a7, 447;" //Load syscall number 447 (SYS_register_user_memory_due_trap_handler) into register a7
                 "ecall;" //Make RISC-V environment call to register our user-defined trap handler
                 :
                 : "r" (entry_trap_fptr));
}

int memory_due_handler_entry(trapframe_t* tf) {
    dueinfo_t user_recovery_context;

    //Init
    user_recovery_context.valid_tf = 0;
    user_recovery_context.error_in_stack = 0;
    user_recovery_context.error_in_text = 0;
    user_recovery_context.error_in_data = 0;
    user_recovery_context.error_in_sdata = 0;
    user_recovery_context.error_in_bss = 0;
    user_recovery_context.error_in_heap = 0;

    if (tf) {
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
        if ((void*)(tf->badvaddr) >= _ftext && (void*)(tf->badvaddr) < _etext)
            user_recovery_context.error_in_text = 1;
        if ((void*)(tf->badvaddr) >= _fdata && (void*)(tf->badvaddr) < _edata)
            user_recovery_context.error_in_data = 1;
        if ((void*)(tf->badvaddr) >= _edata && (void*)(tf->badvaddr) < _fbss)
            user_recovery_context.error_in_sdata = 1;
        if ((void*)(tf->badvaddr) >= _fbss && (void*)(tf->badvaddr) < _end)
            user_recovery_context.error_in_bss = 1;
        user_recovery_context.error_in_heap = 0; //TODO
        
        user_recovery_context.valid_tf = 1;
    }

    //Call user handler if PC in error occurred in the registered PC range
    if ((void*)(tf->epc) >= g_due_trap_region_pc_start && (void*)(tf->epc) < g_due_trap_region_pc_end) {
        if (g_user_trap_fptr) {
            g_user_trap_fptr(&user_recovery_context);
            return 0;
        }
    }
    return 1;
}
