/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef MEMORY_DUE_H
#define MEMORY_DUE_H

#include "minipk.h"

extern void* g_due_trap_region_pc_start;
extern void* g_due_trap_region_pc_end;
extern int g_restart_due_region;

typedef struct {
    trapframe_t tf;
    int valid_tf;
    int error_in_stack;
    int error_in_text;
    int error_in_data;
    int error_in_sdata;
    int error_in_bss;
    int error_in_heap;
} dueinfo_t;

#define REGION_DUE_HANDLE_START(fptr)\
    register_user_memory_due_trap_handler(fptr, &&mystart, &&myend);\
    mystart:;

#define REGION_DUE_HANDLE_END\
    myend:;\
    if (g_restart_due_region == 1) {\
        g_restart_due_region = 0;\
        printf("Restarting DUE trap region!\n");\
        goto *g_due_trap_region_pc_start;\
    }

typedef int (*user_defined_trap_handler)(dueinfo_t* recovery_context); 

//Useful symbols defined by the RISC-V linker script
extern void* _ftext; //Front of code segment
extern void* _etext; //End of code segment
extern void* _fdata; //Front of initialized data segment
extern void* _edata; //End of initialized data segment
extern void* _fbss; //Front of uninitialized data segment
extern void* _end; //End of uninitialized data segment... and address space overall?

void dump_dueinfo(dueinfo_t* dueinfo);
void register_user_memory_due_trap_handler(user_defined_trap_handler fptr, void* pc_start, void* pc_end);
int memory_due_handler_entry(trapframe_t* tf);
#endif
