/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef MINIPK_H
#define MINIPK_H

#include <stddef.h>

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
    long fpr[32];
} float_trapframe_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    char bytes[32]; //Support UP TO 256-bit words
    size_t size;
} word_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    word_t candidate_messages[64]; //Support UP TO 64 candidate messages
    size_t size;
} due_candidates_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    word_t words[32]; //Support UP TO 32 words in a cache line
    size_t blockpos;
    size_t size;
} due_cacheline_t;

typedef int (*user_trap_handler)(trapframe_t*, float_trapframe_t*, due_candidates_t*, due_cacheline_t*, word_t*, short, short, short, short); //Originally defined in riscv-pk/pk/pk.h
void dump_tf(trapframe_t* tf); //Originally defined in riscv-pk/pk/pk.h
int copy_word(word_t* dest, word_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_cacheline(due_cacheline_t* dest, due_cacheline_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_candidates(due_candidates_t* dest, due_candidates_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_trapframe(trapframe_t* dest, trapframe_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_float_trapframe(float_trapframe_t* dest, float_trapframe_t* src); //Originally defined in riscv-pk/pk/pk.h
unsigned decode_rd(long insn); //Originally defined in riscv-pk/pk/pk.h
int load_value_from_message(word_t* recovered_message, word_t* load_value, due_cacheline_t* cl, unsigned load_size, int offset); //Originally defined in riscv-pk/pk/pk.h
int get_float_register(unsigned frd, unsigned long* raw_value); //Originally defined in riscv-pk/pk/pk.h
int set_float_register(unsigned frd, unsigned long raw_value); //Originally defined in riscv-pk/pk/pk.h

extern const char* g_int_regnames[];
extern const char* g_float_regnames[];

#endif
