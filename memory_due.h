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

typedef int (*user_defined_trap_handler)(dueinfo_t* recovery_context); 

int foo(dueinfo_t* recovery_context);

#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define FILE_LINE __FILE__ "_" STRINGIFY(__LINE__)

#define VARIABLE_SCOPE_ADDR_PASTER(x,y) x ## _ ## y ## _addr
#define VARIABLE_SCOPE_ADDR_END_PASTER(x,y) x ## _ ## y ## _addr_end

#define DECL_RECOVERY(variable, scope) \
    void* VARIABLE_SCOPE_ADDR_PASTER(variable,scope) = NULL;

#define DECL_RECOVERY_OBJ(variable, scope) \
    void* VARIABLE_SCOPE_ADDR_PASTER(variable,scope) = NULL; \
    void* VARIABLE_SCOPE_ADDR_END_PASTER(variable,scope) = NULL;

#define EN_RECOVERY(variable, scope) \
    VARIABLE_SCOPE_ADDR_PASTER(variable,scope) = (void*)(&variable);

#define EN_RECOVERY_OBJ(variable, scope, size) \
    VARIABLE_SCOPE_ADDR_PASTER(variable,scope) = (void*)(&variable); \
    VARIABLE_SCOPE_ADDR_END_PASTER(variable,scope) = (void*)(variable)+size;

#define RECOVERY_ADDR(variable, scope) \
    VARIABLE_SCOPE_ADDR_PASTER(variable,scope)

#define RECOVERY_END_ADDR(variable, scope) \
    VARIABLE_SCOPE_ADDR_END_PASTER(variable,scope)

#define BEGIN_DUE_RECOVERY(fptr) \
    register_user_memory_due_trap_handler(fptr, &&mystart, &&myend); \
    mystart:;

#define END_DUE_RECOVERY \
    myend:; \
    if (g_restart_due_region == 1) { \
        g_restart_due_region = 0; \
        printf("Restarting DUE trap region!\n"); \
        goto *g_due_trap_region_pc_start; \
    } \
    register_user_memory_due_trap_handler(NULL, NULL, NULL);

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
