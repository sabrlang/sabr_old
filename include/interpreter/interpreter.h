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
#include "interpreter_data.h"

typedef struct memory_pool_struct memory_pool;
struct memory_pool_struct {
	value* data;
	size_t size;
	size_t index;
};

struct interpreter_struct {
	uint8_t* bytecode;
	size_t bytecode_size;
	deque(value) data_stack;
	deque(value) switch_stack;
	deque(for_data) for_data_stack;
	deque(cs_data) call_stack;
	rbt* global_words;
	deque(cctl_ptr(rbt)) local_words_stack;

	vector(cctl_ptr(vector(uint64_t))) struct_vector;

	mbstate_t convert_state;
	
	deque(size_t) local_memory_size_stack;
	memory_pool memory_pool;
	memory_pool global_memory_pool;
};

typedef struct interpreter_struct interpreter;

bool interpreter_init(interpreter* inter);
void interpreter_del(interpreter* inter);

bool interpreter_memory_pool_init(interpreter* inter, size_t size, size_t global_size);

bool interpreter_load_code(interpreter* inter, char* filename);
bool interpreter_run(interpreter* inter);

bool interpreter_pop(interpreter* inter, value* v);
bool interpreter_push(interpreter* inter, value v);

uint32_t interpreter_set_variable(interpreter* inter, size_t* index, value kwrd, value v);
uint32_t interpreter_ref_variable(interpreter* inter, size_t* index, value kwrd, value* addr);
uint32_t interpreter_call_kwrd(interpreter* inter, size_t* index, value kwrd);
value* interpreter_get_variable_addr(interpreter* inter, value kwrd);

inline bool memory_pool_init(memory_pool* pool, size_t size) {
	pool->data = (value*) malloc(sizeof(size_t) * size);
	pool->size = size;
	pool->index = 0;
	if (pool->data) return true;
	return false;
}

inline void memory_pool_del(memory_pool* pool) {
	free(pool->data);
	pool->data = NULL;
}

inline bool memory_pool_alloc(memory_pool* pool, size_t size) {
	if (pool->index + size >= pool->size) return false;
	pool->index += size;
	return true;
}

inline bool memory_pool_free(memory_pool* pool, size_t size) {
	if (pool->index - size < pool->index) return false;
	pool->index -= size;
	return true;
}

inline value* memory_pool_top(memory_pool* pool) {
	return pool->data + pool->index;
}

#endif