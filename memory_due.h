/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef MEMORY_DUE_H
#define MEMORY_DUE_H

#include "minipk.h"

typedef enum {
    STRICTNESS_DEFAULT,
    STRICTNESS_STRICT,
    STRICTNESS_NUM
} due_region_strictness_t;

typedef struct due_handler due_handler_t; //Forward declaration for circular dependencies
typedef struct dueinfo dueinfo_t; //Forward declaration for circular dependencies

typedef int (*user_defined_trap_handler)(dueinfo_t*);

struct due_handler {
    char name[32];
    user_defined_trap_handler fptr;
    due_region_strictness_t strict;
    void* pc_start;
    void* pc_end;
    int restart;
    int invocations;
};

struct dueinfo {
    int valid;
    trapframe_t tf;
    short error_in_stack;
    short error_in_text;
    short error_in_data;
    short error_in_sdata;
    short error_in_bss;
    short error_in_heap;
    due_candidates_t candidates;
    due_cacheline_t cacheline;
    struct due_handler setup;
    word_t recovered_message;
    char expl[128];
};

#define MAX_REGISTERED_HANDLERS 8
#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define FILE_LINE __FILE__ "_" STRINGIFY(__LINE__)

#define VARIABLE_SCOPE_ADDR_PASTER(x,y) x ## _ ## y ## _addr
#define VARIABLE_SCOPE_ADDR_END_PASTER(x,y) x ## _ ## y ## _addr_end
#define FUNCTION_DUE_RECOVERY_NAME(fname, seqnum) fname ## _ ## seqnum ## _ ## memory_due_handler
#define DUE_RECOVERY_HANDLER(fname,seqnum,...) FUNCTION_DUE_RECOVERY_NAME(fname, seqnum)(__VA_ARGS__)

#define DECL_RECOVERY(scope, variable) \
    void* VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = NULL; \
    void* VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable) = NULL;

#define DECL_RECOVERY_EXTERN(scope, variable) \
    extern void* VARIABLE_SCOPE_ADDR_PASTER(scope, variable); \
    extern void* VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable);

#define EN_RECOVERY(scope, variable, size) \
    VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = (void*)(&variable); \
    VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable) = (void*)(&variable)+size;

#define EN_RECOVERY_PTR(scope, variable, size) \
    VARIABLE_SCOPE_ADDR_PASTER(scope, variable) = (void*)(variable); \
    VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable) = (void*)(variable)+size;

#define RECOVERY_ADDR(scope, variable) \
    VARIABLE_SCOPE_ADDR_PASTER(scope, variable)

#define RECOVERY_END_ADDR(scope, variable) \
    VARIABLE_SCOPE_ADDR_END_PASTER(scope, variable)

#define START_DUE_REGION_LABEL(fname, seqnum) \
    fname ## _ ## seqnum ## _ ## start

#define END_DUE_REGION_LABEL(fname, seqnum) \
    fname ## _ ## seqnum ## _ ## end

#define BEGIN_DUE_RECOVERY(fname, seqnum, strict) \
    push_user_memory_due_trap_handler(STRINGIFY(FUNCTION_DUE_RECOVERY_NAME(fname, seqnum)), FUNCTION_DUE_RECOVERY_NAME(fname, seqnum), &&START_DUE_REGION_LABEL(fname, seqnum), &&END_DUE_REGION_LABEL(fname, seqnum), strict); \
    START_DUE_REGION_LABEL(fname,seqnum):;

#define END_DUE_RECOVERY(fname,seqnum) \
    END_DUE_REGION_LABEL(fname,seqnum):; \
    if (g_handler_stack[g_handler_sp].restart == 1) { \
        g_handler_stack[g_handler_sp].restart = 0; \
        printf("Restarting DUE trap region!\n"); \
        goto *(g_handler_stack[g_handler_sp].pc_start); \
    } \
    pop_user_memory_due_trap_handler();

#define DUE_INFO(fname, seqnum) fname ## _ ## seqnum ## _ ## dueinfo

#define DECL_DUE_INFO(fname, seqnum) \
    dueinfo_t DUE_INFO(fname, seqnum);

#define DECL_DUE_INFO_EXTERN(fname, seqnum) \
    extern dueinfo_t DUE_INFO(fname, seqnum);

#define COPY_DUE_INFO(fname, seqnum, src) \
    if (src) { \
        DUE_INFO(fname, seqnum).valid = src->valid; \
        DUE_INFO(fname, seqnum).tf = src->tf; \
        DUE_INFO(fname, seqnum).error_in_stack = src->error_in_stack; \
        DUE_INFO(fname, seqnum).error_in_text = src->error_in_text; \
        DUE_INFO(fname, seqnum).error_in_data = src->error_in_data; \
        DUE_INFO(fname, seqnum).error_in_sdata = src->error_in_sdata; \
        DUE_INFO(fname, seqnum).error_in_bss = src->error_in_bss; \
        DUE_INFO(fname, seqnum).error_in_heap = src->error_in_heap; \
        for (int i = 0; i < 32; i++) { \
            DUE_INFO(fname, seqnum).setup.name[i] = src->setup.name[i]; \
            if (src->setup.name[i] == '\0') \
                break; \
        } \
        DUE_INFO(fname, seqnum).setup.fptr = src->setup.fptr; \
        DUE_INFO(fname, seqnum).setup.strict = src->setup.strict; \
        DUE_INFO(fname, seqnum).setup.pc_start = src->setup.pc_start; \
        DUE_INFO(fname, seqnum).setup.pc_end = src->setup.pc_end; \
        DUE_INFO(fname, seqnum).setup.restart = src->setup.restart; \
        DUE_INFO(fname, seqnum).setup.invocations = invocations; \
        copy_candidates(&(DUE_INFO(fname, seqnum).candidates), &(src->candidates)); \
        copy_cacheline(&(DUE_INFO(fname, seqnum).cacheline), &(src->cacheline)); \
        copy_word(&(DUE_INFO(fname, seqnum).recovered_message), &(src->recovered_message)); \
        for (int i = 0; i < 128; i++) { \
            DUE_INFO(fname, seqnum).expl[i] = src->expl[i]; \
            if (src->expl[i] == '\0') \
                break; \
        } \
    } else { \
        DUE_INFO(fname, seqnum).valid = 0; \
    }

#define DUE_IN(fname, seqnum, variable) \
    ((void *)(DUE_INFO(fname, seqnum).tf.badvaddr) >= RECOVERY_ADDR(fname, variable) && (void *)(DUE_INFO(fname, seqnum).tf.badvaddr) < RECOVERY_END_ADDR(fname, variable))

#define DUE_IN_SPRINTF(fname, seqnum, variable, type, exp) \
    if (DUE_IN(fname, seqnum, variable)) \
        sprintf(exp, "DUE in %s(), PC %p, memory address %p, type %s, variable %s [%p, %p)\n", #fname, DUE_INFO(fname, seqnum).tf.epc, DUE_INFO(fname, seqnum).tf.badvaddr, #type, #variable, RECOVERY_ADDR(fname, variable), RECOVERY_END_ADDR(fname, variable));

#define INJECT_DUE_INSTRUCTION(start_tick_offset, stop_tick_offset) \
    asm volatile("custom0 0,%0,%1,0;" \
                 : \
                 : "r" (start_tick_offset), "r" (stop_tick_offset));

#define INJECT_DUE_DATA(start_tick_offset, stop_tick_offset) \
    asm volatile("custom1 0,%0,%1,0;" \
                 : \
                 : "r" (start_tick_offset), "r" (stop_tick_offset));



//Useful symbols defined by the RISC-V linker script
extern void* _ftext; //Front of code segment
extern void* _etext; //End of code segment
extern void* _fdata; //Front of initialized data segment
extern void* _edata; //End of initialized data segment
extern void* _fbss; //Front of uninitialized data segment
extern void* _end; //End of uninitialized data segment... and address space overall?
extern due_handler_t g_handler_stack[MAX_REGISTERED_HANDLERS];
extern size_t g_handler_sp;

void dump_dueinfo(dueinfo_t* dueinfo);
void push_user_memory_due_trap_handler(char* name, user_defined_trap_handler fptr, void* pc_start, void* pc_end, due_region_strictness_t strict);
void pop_user_memory_due_trap_handler();
int memory_due_handler_entry(trapframe_t* tf, due_candidates_t* candidates, due_cacheline_t* cacheline, word_t* recovered_message);
void dump_word(word_t* w);
void dump_candidate_messages(due_candidates_t* cd);
void dump_cacheline(due_cacheline_t* cl);
void dump_setup(due_handler_t *setup);
#endif
