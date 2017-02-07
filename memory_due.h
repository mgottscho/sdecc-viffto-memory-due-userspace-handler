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

typedef struct {
    char[8] word;
} word_t;

typedef struct {
    word_t* words;
    int linesz;
    int blockpos;
} due_cacheline_t;

typedef struct {
    word_t* candidate_messages;
    int num_candidate_messages;
} due_candidates_t;

typedef int (*user_defined_trap_handler)(dueinfo_t* recovery_context); 

#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define FILE_LINE __FILE__ "_" STRINGIFY(__LINE__)

#define VARIABLE_SCOPE_ADDR_PASTER(x,y) x ## _ ## y ## _addr
#define VARIABLE_SCOPE_ADDR_END_PASTER(x,y) x ## _ ## y ## _addr_end
#define FUNCTION_DUE_RECOVERY_NAME(fname, seqnum) fname ## _ ## seqnum ## _ ## memory_due_handler
#define DUE_RECOVERY_HANDLER(fname,seqnum,...) FUNCTION_DUE_RECOVERY_NAME(fname, seqnum)(__VA_ARGS__)

#define DECL_RECOVERY_PRIMITIVE(scope, variable) \
    void* VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = NULL;

#define DECL_RECOVERY_OBJECT(scope, variable) \
    void* VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = NULL; \
    void* VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable) = NULL;

#define EN_RECOVERY_PRIMITIVE(scope, variable) \
    VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = (void*)(&variable);

#define EN_RECOVERY_OBJECT(scope, variable, size) \
    VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = (void*)(&variable); \
    VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable) = (void*)(variable)+size;

#define RECOVERY_ADDR(scope, variable) \
    VARIABLE_SCOPE_ADDR_PASTER(scope, variable)

#define RECOVERY_END_ADDR(scope, variable) \
    VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable)

#define START_DUE_REGION_LABEL(fname, seqnum) \
    fname ## _ ## seqnum ## _ ## start

#define END_DUE_REGION_LABEL(fname, seqnum) \
    fname ## _ ## seqnum ## _ ## end

#define BEGIN_DUE_RECOVERY(fname, seqnum) \
    register_user_memory_due_trap_handler(FUNCTION_DUE_RECOVERY_NAME(fname, seqnum), &&START_DUE_REGION_LABEL(fname, seqnum), &&END_DUE_REGION_LABEL(fname, seqnum)); \
    START_DUE_REGION_LABEL(fname,seqnum):;

#define END_DUE_RECOVERY(fname,seqnum) \
    END_DUE_REGION_LABEL(fname,seqnum):; \
    if (g_restart_due_region == 1) { \
        g_restart_due_region = 0; \
        printf("Restarting DUE trap region!\n"); \
        goto *g_due_trap_region_pc_start; \
    }
    //register_user_memory_due_trap_handler(NULL, NULL, NULL);

#define DUE_INFO(fname, seqnum) fname ## _ ## seqnum ## _ ## dueinfo

#define DECL_DUE_INFO(fname, seqnum) \
    dueinfo_t DUE_INFO(fname, seqnum);

#define COPY_DUE_INFO(fname, seqnum, src) \
    DUE_INFO(fname, seqnum).tf = src->tf; \
    DUE_INFO(fname, seqnum).valid_tf = src->valid_tf; \
    DUE_INFO(fname, seqnum).error_in_stack = src->error_in_stack; \
    DUE_INFO(fname, seqnum).error_in_text = src->error_in_text; \
    DUE_INFO(fname, seqnum).error_in_data = src->error_in_data; \
    DUE_INFO(fname, seqnum).error_in_sdata = src->error_in_sdata; \
    DUE_INFO(fname, seqnum).error_in_bss = src->error_in_bss; \
    DUE_INFO(fname, seqnum).error_in_heap = src->error_in_heap; \

#define DUE_AT(fname, seqnum, variable) \
    (void *)(DUE_INFO(fname, seqnum).tf.badvaddr) == RECOVERY_ADDR(fname, variable)

#define DUE_IN(fname, seqnum, variable) \
    (void *)(DUE_INFO(fname, seqnum).tf.badvaddr) >= RECOVERY_ADDR(fname, variable) && (void *)(DUE_INFO(fname, seqnum).tf.badvaddr) < RECOVERY_END_ADDR(fname, variable)

#define DUE_AT_PRINTF(fname, seqnum, variable) \
    if (DUE_AT(fname, seqnum, variable)) \
        printf("DUE in %s(), PC %p, memory address %p, variable %s [%p]\n", #fname, DUE_INFO(fname, seqnum).tf.epc, DUE_INFO(fname, seqnum).tf.badvaddr, #variable, RECOVERY_ADDR(fname, variable));

#define DUE_IN_PRINTF(fname, seqnum, variable) \
    if (DUE_IN(fname, seqnum, variable)) \
        printf("DUE in %s(), PC %p, memory address %p, variable %s [%p, %p)\n", #fname, DUE_INFO(fname, seqnum).tf.epc, DUE_INFO(fname, seqnum).tf.badvaddr, #variable, RECOVERY_ADDR(fname, variable), RECOVERY_END_ADDR(fname, variable));

#define INJECT_DUE_INSTRUCTION \
    asm volatile("custom0 0,0,0,0;");

#define INJECT_DUE_DATA \
    asm volatile("custom1 0,0,0,0;");

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
