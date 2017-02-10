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

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    char byte[8];
} word_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    word_t candidate_messages[32];
    int num_candidate_messages;
} due_candidates_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    word_t words[8];
    int blockpos;
} due_cacheline_t;

typedef int (*user_trap_handler)(trapframe_t*, due_candidates_t*, due_cacheline_t*); //Originally defined in riscv-pk/pk/pk.h
void dump_tf(trapframe_t* tf); //Originally defined in riscv-pk/pk/pk.h

#endif
