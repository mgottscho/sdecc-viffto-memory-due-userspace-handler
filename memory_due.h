/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef MEMORY_DUE_H
#define MEMORY_DUE_H

//Originally defined in riscv-pk/include/pk.h
typedef struct {
    long gpr[32];
    long status;
    long epc;
    long badvaddr;
    long cause;
    long insn;
} trapframe_t;

typedef struct {
    trapframe_t tf;
    //TODO: candidate codewords?
    int error_in_stack;
    int error_in_text;
    int error_in_data;
    int error_in_bss;
    int error_in_heap;
} dueinfo_t;


typedef void (*trap_handler)(trapframe_t* tf); //Originally defined in riscv-pk/include/pk.h
typedef void (*user_trap_handler)(dueinfo_t* recovery_context); 

//Useful symbols defined by the RISC-V linker script
extern char _ftext; //Front of code segment
extern char _etext; //End of code segment
extern char _fdata; //Front of initialized data segment
extern char _edata; //End of initialized data segment
extern char _fbss; //Front of uninitialized data segment
extern char _end; //End of uninitialized data segment... and address space overall?

void dump_tf(trapframe_t* tf); //Originally defined in riscv-pk/pk/console.c
void dump_dueinfo(dueinfo_t* dueinfo);
void register_user_memory_due_trap_handler(user_trap_handler fptr);
void default_user_memory_due_trap_handler(dueinfo_t* recovery_context);
void memory_due_handler_entry(trapframe_t* tf);
#endif
