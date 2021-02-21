#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <inttypes.h>
#include <math.h>
#include <uchar.h>

#include "opcode.h"
#include "rbt.h"
#include "value.h"

#include "interpreter_cctl_define.h"

typedef struct interpreter_struct {
	uint8_t* bytecode;
	size_t bytecode_size;
	vector(value) data_stack;
	vector(size_t) call_stack;
	rbt* global_words;
	vector(cctl_ptr(rbt)) local_words_stack;
} interpreter;

void interpreter_init(interpreter* inter);
void interpreter_del(interpreter* inter);
bool interpreter_load_code(interpreter* inter, char* filename);
bool interpreter_run(interpreter* inter);

bool interpreter_pop(interpreter* inter, value* v);
bool interpreter_push(interpreter* inter, value v);

#endif