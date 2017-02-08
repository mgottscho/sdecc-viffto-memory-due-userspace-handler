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
    if (dueinfo && dueinfo->valid) {
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
        dump_candidate_messages(&(dueinfo->candidates));
        dump_cacheline(&(dueinfo->cacheline));
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

int memory_due_handler_entry(trapframe_t* tf, due_candidates_t* candidates, due_cacheline_t* cacheline) {
    dueinfo_t user_recovery_context;

    //Init
    user_recovery_context.valid = 0;
    user_recovery_context.error_in_stack = 0;
    user_recovery_context.error_in_text = 0;
    user_recovery_context.error_in_data = 0;
    user_recovery_context.error_in_sdata = 0;
    user_recovery_context.error_in_bss = 0;
    user_recovery_context.error_in_heap = 0;
    user_recovery_context.candidates.candidate_messages = NULL;
    user_recovery_context.candidates.num_candidate_messages = 0;
    user_recovery_context.cacheline.words = NULL;
    user_recovery_context.cacheline.linesz = 0;
    user_recovery_context.cacheline.blockpos = 0;

    if (tf) {
        //Copy trap frame
        for (int i = 0; i < 32; i++)
            user_recovery_context.tf.gpr[i] = tf->gpr[i];
        user_recovery_context.tf.status = tf->status;
        user_recovery_context.tf.epc = tf->epc;
        user_recovery_context.tf.badvaddr = tf->badvaddr;
        user_recovery_context.tf.cause = tf->cause;
        user_recovery_context.tf.insn = tf->insn;

        //Analyze trap frame, determine in which segment the memory DUE occured
        if (tf->badvaddr >= tf->gpr[2]-1024 && tf->badvaddr < tf->gpr[2]+1024) //gpr[2] is sp. FIXME: how to find size of stack frame dynamically, or otherwise find the base of stack?
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
        
    }

    if (candidates) {
        //Copy candidate messages
        //FIXME: memcpy?
        word_t candidate_messages[candidates->num_candidate_messages];
        for (int i = 0; i < candidates->num_candidate_messages; i++) {
            for (int j = 0; j < 8; j++)
                candidate_messages[i].byte[j] = (candidates->candidate_messages[i]).byte[j];
        }
        user_recovery_context.candidates.candidate_messages = candidate_messages;
        user_recovery_context.candidates.num_candidate_messages = candidates->num_candidate_messages;
    }

    if (cacheline) {
        //Copy cacheline
        //FIXME: memcpy?
        due_cacheline_t cl;
        for (int i = 0; i < cacheline->linesz; i++) {
            for (int j = 0; j < 8; j++)
                cl.words[i].byte[j] = cacheline->words[i].byte[j];
        }
        cl.linesz = cacheline->linesz;
        cl.blockpos = cacheline->blockpos;
        user_recovery_context.cacheline = cl;
    }
        
    user_recovery_context.valid = 1;

    //Call user handler if PC in error occurred in the registered PC range
    if ((void*)(tf->epc) >= g_due_trap_region_pc_start && (void*)(tf->epc) < g_due_trap_region_pc_end) {
        if (g_user_trap_fptr) {
            g_user_trap_fptr(&user_recovery_context);
            return 0;
        }
    }
    return 1;
}

void dump_candidate_messages(due_candidates_t* cd) {
   if (cd) {
       for (int i = 0; i < cd->num_candidate_messages; i++) {
           printf("Candidate message %d: 0x", i);
           for (int j = 0; j < 8; j++)
               printf("%x", cd->candidate_messages[i].byte[j]);
           printf("\n");
       }
   } else
       printf("Invalid candidate messages!\n");
}

void dump_cacheline(due_cacheline_t* cl) {
   if (cl) {
       for (int i = 0; i < cl->linesz; i++) {
           if (cl->blockpos != i) {
               printf("Word %d: 0x", i);
               for (int j = 0; j < 8; j++)
                   printf("%x", cl->words[i].byte[j]);
           } else
               printf("Word %d: <CORRUPTED MESSAGE>", i);
           printf("\n");
       }
   } else
       printf("Invalid cacheline!\n");
}
