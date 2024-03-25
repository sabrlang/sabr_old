#include "interpreter.h"
#include "interpreter_op.h"

extern inline bool memory_pool_init(memory_pool* pool, size_t size);
extern inline void memory_pool_del(memory_pool* pool);
extern inline bool memory_pool_alloc(memory_pool* pool, size_t size);
extern inline bool memory_pool_free(memory_pool* pool, size_t size);
extern inline value* memory_pool_top(memory_pool* pool);

bool interpreter_init(interpreter* inter) {
	setlocale(LC_ALL, "en_US.utf8");

#if defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	_setmode(_fileno(stdin), _O_U16TEXT);
	fflush(stdin);
#endif

	deque_init(value, &inter->data_stack);
	deque_init(value, &inter->switch_stack);
	deque_init(for_data, &inter->for_data_stack);
	deque_init(cs_data, &inter->call_stack);

	deque_init(cctl_ptr(rbt), &inter->local_words_stack);
	inter->global_words = rbt_new();
	if (!(inter->global_words)) {
		fputs("error : Dictionary memory allocation failure\n", stderr);
		return false;
	}

	vector_init(cctl_ptr(vector(uint64_t)), &inter->struct_vector);

	deque_init(size_t, &inter->local_memory_size_stack);
	deque_push_back(size_t, &inter->local_memory_size_stack, 0);

	return true;
}

void interpreter_del(interpreter* inter) {
	deque_free(value, &inter->data_stack);
	deque_free(value, &inter->switch_stack);
	deque_free(for_data, &inter->for_data_stack);
	deque_free(cs_data, &inter->call_stack);
	
	for (size_t i = 0; i < inter->local_words_stack.size; i++) {
		rbt_free(*deque_at(cctl_ptr(rbt), &inter->local_words_stack, i));
	}
	deque_free(cctl_ptr(rbt), &inter->local_words_stack);
	rbt_free(inter->global_words);

	for (size_t i = 0; i < inter->struct_vector.size; i++) {
		vector_free(uint64_t, *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, i));
	}
	vector_free(cctl_ptr(vector(uint64_t)), &inter->struct_vector);

	deque_free(size_t, &inter->local_memory_size_stack);

	memory_pool_del(&inter->memory_pool);
	memory_pool_del(&inter->global_memory_pool);
}

bool interpreter_memory_pool_init(interpreter* inter, size_t size, size_t global_size) {
	if (!memory_pool_init(&inter->memory_pool, size)) return false;
	if (!memory_pool_init(&inter->global_memory_pool, global_size)) return false;
	return true;
}

bool interpreter_load_code(interpreter* inter, char* filename) {
	FILE* file;
	size_t size;

	file = fopen(filename, "rb");
	if (!file) {
		fputs("error : File reading failure\n", stderr);
		return false;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	uint8_t* code = (uint8_t*) malloc(size);
	if (!code) {
		fclose(file);
		fputs("error : Memory allocation failure\n", stderr);
		return false;
	}

	int a = fread(code, size, 1, file);
	if (a != 1) {
		free(code);
		fclose(file);
		fputs("error : Entire reading failure\n", stderr);
		return false;
	}

	inter->bytecode = code;
	inter->bytecode_size = size;

	fclose(file);
	return true;
}

bool interpreter_run(interpreter* inter) {
	uint8_t* code;
	uint32_t errcode;

	size_t index, index_temp;

	for (index = 0; index < inter->bytecode_size; index++) {
		code = inter->bytecode + index;
		if ((*code > interpreter_op_functions_len)) {
			fprintf(stderr, "\'%u\'\n", *code);
			fputs("error : Invalid operation code\n", stderr);
			goto FAILURE;
		}
		if (*code == 0) {
			continue;
		}
		index_temp = index;
		errcode = interpreter_op_functions[*code - 1](inter, &index);
		switch (errcode) {
			case OPERR_NONE:
				break;
			case OPERR_STACK:
				fputs("error : Stack memory error\n", stderr);
				goto FAILURE;
				break;
			case OPERR_SWITCH:
				fputs("error : Switch stack error\n", stderr);
				goto FAILURE;
				break;
			case OPERR_REDEFINE:
				fputs("error : Redefined keyword\n", stderr);
				goto FAILURE;
				break;
			case OPERR_DEFINE:
				fputs("error : Definition failure\n", stderr);
				goto FAILURE;
				break;
			case OPERR_UNDEFINED:
				fputs("error : Undefined keyword\n", stderr);
				goto FAILURE;
				break;
			case OPERR_CALL:
				fputs("error : Call stack error\n", stderr);
				goto FAILURE;
				break;
			case OPERR_STRUCT:
				fputs("error : Struct definition failure", stderr);
				goto FAILURE;
				break;
			case OPERR_INVALID_KWRD:
				fputs("error : Invalid keyword\n", stderr);
				goto FAILURE;
				break;
			case OPERR_DIV_BY_ZERO:
				fputs("error : Division by zero\n", stderr);
				goto FAILURE;
				break;
			case OPERR_STDIN:
				fputs("error: Input error\n", stderr);
				goto FAILURE;
				break;
			case OPERR_UNICODE:
				fputs("error : Unicode encoding failure\n", stderr);
				goto FAILURE;
				break;
			case OPERR_MEMORY:
				fputs("error : Memory allocation failure\n", stderr);
				goto FAILURE;
				break;
		}
	}
	return true;
FAILURE:
	fprintf(stderr, "at index %zu (%zx)\n", index_temp, index_temp);
	return false;
}

bool interpreter_pop(interpreter* inter, value* v) {
	if (!inter->data_stack.size) {
		fputs("error : Stack underflow\n", stderr);
		return false;
	}
	*v = *deque_back(value, &inter->data_stack);
	deque_pop_back(value, &inter->data_stack);
	return true;
}

bool interpreter_push(interpreter* inter, value v) {
	if (!deque_push_back(value, &inter->data_stack, v)) {
		fputs("error : Stack memory allocation failure\n", stderr);
		return false;
	}
	return true;
}

uint32_t interpreter_set_variable(interpreter* inter, size_t* index, value kwrd, value v) {
	rbt* words = NULL;
	rbt_node* node = NULL;

	bool is_global = false;

	node = rbt_search(inter->global_words, kwrd.u);
	if (!node) {
		if (inter->local_words_stack.size > 0) {
			words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
			node = rbt_search(words, kwrd.u);
		}
		else {
			words = inter->global_words;
			is_global = true;
		}
	}
	if (!node) {
		node = rbt_node_new(kwrd.u);
		if (!node) return OPERR_DEFINE;
		node->type = KWRD_VAR;
		
		value* p;
		if (is_global) {
			p = memory_pool_top(&inter->global_memory_pool);
			if (!memory_pool_alloc(&inter->global_memory_pool, 1)) return OPERR_MEMORY;
		}
		else {
			p = memory_pool_top(&inter->memory_pool);
			if (!memory_pool_alloc(&inter->memory_pool, 1)) return OPERR_MEMORY;
			size_t* local_memory_size = deque_back(size_t, &inter->local_memory_size_stack);
			if (!local_memory_size) return OPERR_MEMORY;
			*local_memory_size += 1;
		}
		node->data = (size_t) p;

		rbt_insert(words, node);
	}

	if (node->type == KWRD_VAR) {
		value* p = (value*) node->data;
		p->u = v.u;
	}
	else return OPERR_INVALID_KWRD;

	return OPERR_NONE;
}


uint32_t interpreter_ref_variable(interpreter* inter, size_t* index, value kwrd, value* addr) {
	rbt* words = NULL;
	rbt_node* node = NULL;

	node = rbt_search(inter->global_words, kwrd.u);
	if (!node) {
		if (inter->local_memory_size_stack.size > 0) {
			words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
			node = rbt_search(words, kwrd.u);
		}
		else {
			words = inter->global_words;
		}
	}
	if (!node) {
		node = rbt_node_new(kwrd.u);
		if (!node) return OPERR_DEFINE;
		node->type = KWRD_VAR;

		value* p = addr;
		node->data = (size_t) p;

		rbt_insert(words, node);
	}
	else return OPERR_REDEFINE;

	return OPERR_NONE;
}

uint32_t interpreter_call_kwrd(interpreter* inter, size_t* index, value kwrd) {
	rbt* local_words = NULL;
	rbt_node* node = NULL;

	node = rbt_search(inter->global_words, kwrd.u);
	if (!node) {
		if (inter->local_words_stack.size > 0) {
			local_words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
			node = rbt_search(local_words, kwrd.u);
		}
	}

	if (!node) return OPERR_UNDEFINED;
	
	cs_data csd;

	switch (node->type) {
		case KWRD_FUNC: {
			csd.pos = *index + 1;
			csd.for_data_stack_size = inter->for_data_stack.size;
			csd.switch_stack_size = inter->switch_stack.size;

			if (!deque_push_back(cs_data, &inter->call_stack, csd)) return OPERR_CALL;
			local_words = rbt_new();
			if (!local_words) return OPERR_CALL;
			if (!deque_push_back(cctl_ptr(rbt), &inter->local_words_stack, local_words)) return OPERR_CALL;

			deque_push_back(size_t, &inter->local_memory_size_stack, 0);

			*index = node->data - 1;
		} break;
		case KWRD_MACRO: {
			csd.pos = *index + 1;
			csd.for_data_stack_size = inter->for_data_stack.size;
			csd.switch_stack_size = inter->switch_stack.size;
			
			if (!deque_push_back(cs_data, &inter->call_stack, csd)) return OPERR_CALL;
			*index = node->data - 1;
		} break;
		case KWRD_VAR: {
			value v;
			value* p = (value*) node->data;
			v.u = p->u;
			if (!interpreter_push(inter, v)) return OPERR_STACK;
		} break;
		case KWRD_STRUCT: {
			value v;
			vector(uint64_t)* temp_struct = *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, node->data);
			if (!temp_struct) return OPERR_STRUCT;
			v.u = temp_struct->size * sizeof(value);
			if (!interpreter_push(inter, v)) return OPERR_STACK;
		} break;
	}

	return OPERR_NONE;
}

value* interpreter_get_variable_addr(interpreter* inter, value kwrd) {
	rbt* local_words = NULL;
	rbt_node* node;
	node = rbt_search(inter->global_words, kwrd.u);
	if (!node) {
		if (inter->local_words_stack.size > 0) {
			local_words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
			node = rbt_search(local_words, kwrd.u);
		}
	}

	if (!node) return NULL;
	if (node->type != KWRD_VAR) return NULL;
	return (value*) node->data;
}