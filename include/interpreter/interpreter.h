#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <inttypes.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <uchar.h>

#if defined(_WIN32)
	#include <fcntl.h>
	#include <io.h>
	#include <windows.h>
#else
	#include <sys/types.h>
#endif

#include "opcode.h"
#include "rbt.h"
#include "value.h"
#include "encoding.h"

#include "interpreter_cctl_define.h"

typedef struct interpreter_struct {
	uint8_t* bytecode;
	size_t bytecode_size;
	deque(value) data_stack;
	deque(value) switch_stack;
	deque(size_t) call_stack;
	rbt* global_words;
	deque(cctl_ptr(rbt)) local_words_stack;
	deque(cctl_ptr(vector(value))) local_memories_stack;
	mbstate_t convert_state;

	vector(cctl_ptr(vector(uint64_t))) struct_vector;

} interpreter;

bool interpreter_init(interpreter* inter);
void interpreter_del(interpreter* inter);
bool interpreter_load_code(interpreter* inter, char* filename);
bool interpreter_run(interpreter* inter);

bool interpreter_pop(interpreter* inter, value* v);
bool interpreter_push(interpreter* inter, value v);

#endif