/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef MINIPK_H
#define MINIPK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define NUM_GPR 32
#define NUM_FPR 32
#define MAX_CANDIDATE_MSG 64
#define MAX_CACHELINE_WORDS 32
#define MAX_WORD_SIZE 32

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    long gpr[NUM_GPR];
    long status;
    long epc;
    long badvaddr;
    long cause;
    long insn;
} trapframe_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    long fpr[NUM_FPR];
} float_trapframe_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    unsigned char bytes[MAX_WORD_SIZE]; 
    size_t size;
} word_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    word_t candidate_messages[MAX_CANDIDATE_MSG];
    size_t size;
} due_candidates_t;

//Originally defined in riscv-pk/pk/pk.h
typedef struct {
    word_t words[MAX_CACHELINE_WORDS];
    size_t blockpos;
    size_t size;
} due_cacheline_t;

typedef int (*user_trap_handler)(trapframe_t*, float_trapframe_t*, long, due_candidates_t*, due_cacheline_t*, word_t*, size_t, size_t, int, int, int); //Originally defined in riscv-pk/pk/pk.h
void dump_tf(trapframe_t* tf); //Originally defined in riscv-pk/pk/pk.h
int copy_word(word_t* dest, word_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_cacheline(due_cacheline_t* dest, due_cacheline_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_candidates(due_candidates_t* dest, due_candidates_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_trapframe(trapframe_t* dest, trapframe_t* src); //Originally defined in riscv-pk/pk/pk.h
int copy_float_trapframe(float_trapframe_t* dest, float_trapframe_t* src); //Originally defined in riscv-pk/pk/pk.h
int load_value_from_message(word_t* recovered_message, word_t* load_value, due_cacheline_t* cl, size_t load_size, int offset); //Originally defined in riscv-pk/pk/pk.h
int get_float_register(size_t frd, unsigned long* raw_value); //Originally defined in riscv-pk/pk/pk.h
int set_float_register(size_t frd, unsigned long raw_value); //Originally defined in riscv-pk/pk/pk.h

extern const char* g_int_regnames[];
extern const char* g_float_regnames[];

#ifdef __cplusplus
} // extern "C"
#endif
#endif
