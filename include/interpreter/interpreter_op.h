#include "interpreter.h"

typedef enum interpreter_op_errcode_enum {
	OPERR_NONE,
	OPERR_STACK,
	OPERR_SWITCH,
	OPERR_REDEFINE,
	OPERR_DEFINE,
	OPERR_UNDEFINED,
	OPERR_CALL,
	OPERR_STRUCT,
	OPERR_INVALID_KWRD,
	OPERR_DIV_BY_ZERO,
	OPERR_STDIN,
	OPERR_UNICODE
} interpreter_op_errcode;

uint32_t interpreter_op_value(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_if(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_jump(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_switch(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_case(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_end_switch(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_func(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_macro(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_return_func(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_return_macro(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_struct(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_member(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_call_member(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_to(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_call(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_add(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_sub(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_mul(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_div(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_mod(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_udiv(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_umod(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_neg(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_inc(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_dec(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_equ(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_neq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_grt(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_geq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_lst(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_leq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_ugrt(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_ugeq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_ulst(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_uleq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fadd(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fsub(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fmul(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fdiv(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fmod(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fneg(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fequ(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fneq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fgrt(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fgeq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_flst(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fleq(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_and(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_or(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_xor(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_not(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_lsft(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_rsft(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_drop(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_nip(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_dup(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_over(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_tuck(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_swap(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_rot(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_tdrop(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_tnip(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_tdup(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_tover(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_ttuck(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_tswap(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_trot(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_alloc(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_resize(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_free(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_fetch(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_store(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_stof(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_utof(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_ftos(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_ftou(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_geti(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_getu(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_getf(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_gets(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_putc(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_puti(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_putu(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_putf(interpreter* inter, uint8_t* code, size_t* index);
uint32_t interpreter_op_show(interpreter* inter, uint8_t* code, size_t* index);