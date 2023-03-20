#ifndef __INTERPRETER_OP_H__
#define __INTERPRETER_OP_H__

#include "interpreter.h"

typedef struct interpreter_struct interpreter;

typedef enum interpreter_op_errcode_enum {
	OPERR_NONE,
	OPERR_STACK,
	OPERR_SWITCH,
	OPERR_FOR,
	OPERR_REDEFINE,
	OPERR_DEFINE,
	OPERR_UNDEFINED,
	OPERR_CALL,
	OPERR_STRUCT,
	OPERR_INVALID_KWRD,
	OPERR_DIV_BY_ZERO,
	OPERR_STDIN,
	OPERR_UNICODE,
	OPERR_MEMORY
} interpreter_op_errcode;

extern size_t interpreter_op_functions_len;
extern const uint32_t (*interpreter_op_functions[])(interpreter*, size_t*);

uint32_t interpreter_op_exit(interpreter* inter, size_t* index);
uint32_t interpreter_op_value(interpreter* inter, size_t* index);

uint32_t interpreter_op_if(interpreter* inter, size_t* index);
uint32_t interpreter_op_jump(interpreter* inter, size_t* index);

uint32_t interpreter_op_for(interpreter* inter, size_t* index);
uint32_t interpreter_op_for_from(interpreter* inter, size_t* index);
uint32_t interpreter_op_for_to(interpreter* inter, size_t* index);
uint32_t interpreter_op_for_step(interpreter* inter, size_t* index);
uint32_t interpreter_op_for_check(interpreter* inter, size_t* index);
uint32_t interpreter_op_for_incjmp(interpreter* inter, size_t* index);
uint32_t interpreter_op_end_for(interpreter* inter, size_t* index);

uint32_t interpreter_op_switch(interpreter* inter, size_t* index);
uint32_t interpreter_op_case(interpreter* inter, size_t* index);
uint32_t interpreter_op_end_switch(interpreter* inter, size_t* index);

uint32_t interpreter_op_func(interpreter* inter, size_t* index);
uint32_t interpreter_op_macro(interpreter* inter, size_t* index);

uint32_t interpreter_op_callable(interpreter* inter, size_t* index);

uint32_t interpreter_op_return_func(interpreter* inter, size_t* index);
uint32_t interpreter_op_return_macro(interpreter* inter, size_t* index);
uint32_t interpreter_op_struct(interpreter* inter, size_t* index);
uint32_t interpreter_op_member(interpreter* inter, size_t* index);
uint32_t interpreter_op_end_struct(interpreter* inter, size_t* index);
uint32_t interpreter_op_call_member(interpreter* inter, size_t* index);
uint32_t interpreter_op_set(interpreter* inter, size_t* index);
uint32_t interpreter_op_call(interpreter* inter, size_t* index);
uint32_t interpreter_op_addr(interpreter* inter, size_t* index);
uint32_t interpreter_op_ref(interpreter* inter, size_t* index);
uint32_t interpreter_op_add(interpreter* inter, size_t* index);
uint32_t interpreter_op_sub(interpreter* inter, size_t* index);
uint32_t interpreter_op_mul(interpreter* inter, size_t* index);
uint32_t interpreter_op_div(interpreter* inter, size_t* index);
uint32_t interpreter_op_mod(interpreter* inter, size_t* index);
uint32_t interpreter_op_udiv(interpreter* inter, size_t* index);
uint32_t interpreter_op_umod(interpreter* inter, size_t* index);
uint32_t interpreter_op_neg(interpreter* inter, size_t* index);
uint32_t interpreter_op_inc(interpreter* inter, size_t* index);
uint32_t interpreter_op_dec(interpreter* inter, size_t* index);
uint32_t interpreter_op_equ(interpreter* inter, size_t* index);
uint32_t interpreter_op_neq(interpreter* inter, size_t* index);
uint32_t interpreter_op_grt(interpreter* inter, size_t* index);
uint32_t interpreter_op_geq(interpreter* inter, size_t* index);
uint32_t interpreter_op_lst(interpreter* inter, size_t* index);
uint32_t interpreter_op_leq(interpreter* inter, size_t* index);
uint32_t interpreter_op_ugrt(interpreter* inter, size_t* index);
uint32_t interpreter_op_ugeq(interpreter* inter, size_t* index);
uint32_t interpreter_op_ulst(interpreter* inter, size_t* index);
uint32_t interpreter_op_uleq(interpreter* inter, size_t* index);
uint32_t interpreter_op_fadd(interpreter* inter, size_t* index);
uint32_t interpreter_op_fsub(interpreter* inter, size_t* index);
uint32_t interpreter_op_fmul(interpreter* inter, size_t* index);
uint32_t interpreter_op_fdiv(interpreter* inter, size_t* index);
uint32_t interpreter_op_fmod(interpreter* inter, size_t* index);
uint32_t interpreter_op_fneg(interpreter* inter, size_t* index);
uint32_t interpreter_op_fequ(interpreter* inter, size_t* index);
uint32_t interpreter_op_fneq(interpreter* inter, size_t* index);
uint32_t interpreter_op_fgrt(interpreter* inter, size_t* index);
uint32_t interpreter_op_fgeq(interpreter* inter, size_t* index);
uint32_t interpreter_op_flst(interpreter* inter, size_t* index);
uint32_t interpreter_op_fleq(interpreter* inter, size_t* index);
uint32_t interpreter_op_and(interpreter* inter, size_t* index);
uint32_t interpreter_op_or(interpreter* inter, size_t* index);
uint32_t interpreter_op_xor(interpreter* inter, size_t* index);
uint32_t interpreter_op_not(interpreter* inter, size_t* index);
uint32_t interpreter_op_lsft(interpreter* inter, size_t* index);
uint32_t interpreter_op_rsft(interpreter* inter, size_t* index);
uint32_t interpreter_op_drop(interpreter* inter, size_t* index);
uint32_t interpreter_op_nip(interpreter* inter, size_t* index);
uint32_t interpreter_op_dup(interpreter* inter, size_t* index);
uint32_t interpreter_op_over(interpreter* inter, size_t* index);
uint32_t interpreter_op_tuck(interpreter* inter, size_t* index);
uint32_t interpreter_op_swap(interpreter* inter, size_t* index);
uint32_t interpreter_op_rot(interpreter* inter, size_t* index);
uint32_t interpreter_op_tdrop(interpreter* inter, size_t* index);
uint32_t interpreter_op_tnip(interpreter* inter, size_t* index);
uint32_t interpreter_op_tdup(interpreter* inter, size_t* index);
uint32_t interpreter_op_tover(interpreter* inter, size_t* index);
uint32_t interpreter_op_ttuck(interpreter* inter, size_t* index);
uint32_t interpreter_op_tswap(interpreter* inter, size_t* index);
uint32_t interpreter_op_trot(interpreter* inter, size_t* index);
uint32_t interpreter_op_alloc(interpreter* inter, size_t* index);
uint32_t interpreter_op_resize(interpreter* inter, size_t* index);
uint32_t interpreter_op_free(interpreter* inter, size_t* index);
uint32_t interpreter_op_allot(interpreter* inter, size_t* index);
uint32_t interpreter_op_fetch(interpreter* inter, size_t* index);
uint32_t interpreter_op_store(interpreter* inter, size_t* index);
uint32_t interpreter_op_stof(interpreter* inter, size_t* index);
uint32_t interpreter_op_utof(interpreter* inter, size_t* index);
uint32_t interpreter_op_ftos(interpreter* inter, size_t* index);
uint32_t interpreter_op_ftou(interpreter* inter, size_t* index);
uint32_t interpreter_op_geti(interpreter* inter, size_t* index);
uint32_t interpreter_op_getu(interpreter* inter, size_t* index);
uint32_t interpreter_op_getf(interpreter* inter, size_t* index);
uint32_t interpreter_op_gets(interpreter* inter, size_t* index);
uint32_t interpreter_op_putc(interpreter* inter, size_t* index);
uint32_t interpreter_op_puti(interpreter* inter, size_t* index);
uint32_t interpreter_op_putu(interpreter* inter, size_t* index);
uint32_t interpreter_op_putf(interpreter* inter, size_t* index);
uint32_t interpreter_op_show(interpreter* inter, size_t* index);

#endif