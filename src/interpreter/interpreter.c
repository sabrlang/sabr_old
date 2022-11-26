#include "interpreter.h"
#include "interpreter_op.h"

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
	deque_init(size_t, &inter->call_stack);

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
	deque_free(size_t, &inter->call_stack);
	
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

bool memory_pool_init(memory_pool* pool, size_t size) {
	pool->data = (value*) malloc(sizeof(size_t) * size);
	pool->size = size;
	pool->index = 0;
	if (pool->data) return false;
	return true;
}

void memory_pool_del(memory_pool* pool) {
	free(pool->data);
	pool->data = NULL;
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

	for (size_t index = 0; index < inter->bytecode_size; index++) {
		code = inter->bytecode + index;
		if ((*code == 0) || (*code > interpreter_op_functions_len)) {
			fprintf(stderr, "\'%u\'\n", *code);
			fputs("error : Invalid operation code\n", stderr);
			return false;
		}
		errcode = interpreter_op_functions[*code - 1](inter, &index);
		switch (errcode) {
			case OPERR_NONE:
				break;
			case OPERR_STACK:
				fputs("error : Stack memory error\n", stderr);
				return false;
				break;
			case OPERR_SWITCH:
				fputs("error : Switch stack error\n", stderr);
				return false;
				break;
			case OPERR_REDEFINE:
				fputs("error : Redefined keyword\n", stderr);
				return false;
				break;
			case OPERR_DEFINE:
				fputs("error : Definition failure\n", stderr);
				return false;
				break;
			case OPERR_UNDEFINED:
				fputs("error : Undefined keyword\n", stderr);
				return false;
				break;
			case OPERR_CALL:
				fputs("error : Call stack error\n", stderr);
				return false;
				break;
			case OPERR_STRUCT:
				fputs("error : Struct definition failure", stderr);
				return false;
				break;
			case OPERR_INVALID_KWRD:
				fputs("error : Invalid keyword\n", stderr);
				return false;
				break;
			case OPERR_DIV_BY_ZERO:
				fputs("error : Division by zero\n", stderr);
				return false;
				break;
			case OPERR_STDIN:
				fputs("error: Input error\n", stderr);
				return false;
				break;
			case OPERR_UNICODE:
				fputs("error : Unicode encoding failure\n", stderr);
				return false;
				break;
			case OPERR_MEMORY:
				fputs("error : Memory allocation failure\n", stderr);
				return false;
				break;
		}
	}
	return true;
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