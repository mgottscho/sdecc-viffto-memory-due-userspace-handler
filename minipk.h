/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef MINIPK_H
#define MINIPK_H

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    long gpr[32];
    long status;
    long epc;
    long badvaddr;
    long cause;
    long insn;
} trapframe_t;

typedef int (*user_trap_handler)(trapframe_t* tf); //Originally defined in riscv-pk/pk/pk.h
void dump_tf(trapframe_t* tf); //Originally defined in riscv-pk/pk/pk.h

#endif
