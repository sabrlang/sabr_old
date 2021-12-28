#include "interpreter.h"

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

	for (size_t index = 0; index < inter->bytecode_size; index++) {
		code = inter->bytecode + index;
		switch (*code) {
			case OP_VALUE: {
				value v;
				for (int i = 0; i < 8; i++) {
					v.bytes[i] = inter->bytecode[++index];
				}
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_IF: {
				value pos;
				value v;
				for (int i = 0; i < 8; i++) {
					pos.bytes[i] = inter->bytecode[++index];
				}
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				if (!v.u) index = pos.u - 1;
			} break;
			case OP_JUMP: {
				value pos;
				for (int i = 0; i < 8; i++) {
					pos.bytes[i] = inter->bytecode[++index];
				}
				index = pos.u - 1;
			} break;
			case OP_SWITCH: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				if (!deque_push_back(value, &inter->switch_stack, v)) goto FAILURE_SWITCH;
			} break;
			case OP_CASE: {
				value v;
				v = *deque_back(value, &inter->switch_stack);
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_END_SWITCH: {
				if (!deque_pop_back(value, &inter->switch_stack)) goto FAILURE_SWITCH;
			} break;
			case OP_FUNC: {
				value kwrd;
				value pos;

				if (!interpreter_pop(inter, &kwrd)) goto FAILURE_STACK;
				rbt_node* node = NULL;
				node = rbt_search(inter->global_words, kwrd.u);
				if (node) goto FAILURE_REDEFINE;
				node = rbt_node_new(kwrd.u);
				if (!node) goto FAILURE_DEFINE;

				node->data = index + 9;
				node->type = KWRD_FUNC;
				rbt_insert(inter->global_words, node);

				for (int i = 0; i < 8; i++) {
					pos.bytes[i] = inter->bytecode[++index];
				}
				index = pos.u - 1;
			} break;
			case OP_MACRO: {
				value kwrd;
				value pos;
				if (!interpreter_pop(inter, &kwrd)) goto FAILURE_STACK;
				rbt_node* node = NULL;
				node = rbt_search(inter->global_words, kwrd.u);
				if (node) goto FAILURE_REDEFINE;
				node = rbt_node_new(kwrd.u);
				if (!node) goto FAILURE_DEFINE;

				node->data = index + 9;
				node->type = KWRD_MACRO;
				rbt_insert(inter->global_words, node);

				for (int i = 0; i < 8; i++) {
					pos.bytes[i] = inter->bytecode[++index];
				}
				index = pos.u - 1;
			} break;
			case OP_RETURN_FUNC: {
				if (inter->call_stack.size < 1) goto FAILURE_CALL;
				size_t pos = *deque_back(size_t, &inter->call_stack);
				if (!deque_pop_back(size_t, &inter->call_stack)) goto FAILURE_CALL;
				rbt* local_words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
				rbt_free(local_words);
				if (!deque_pop_back(cctl_ptr(rbt), &inter->local_words_stack)) goto FAILURE_CALL;
				index = pos - 1;
			} break;
			case OP_RETURN_MACRO: {
				if (inter->call_stack.size < 1) goto FAILURE_CALL;
				size_t pos = *deque_back(size_t, &inter->call_stack);
				if (!deque_pop_back(size_t, &inter->call_stack)) goto FAILURE_CALL;
				index = pos - 1;
			} break;
			case OP_STRUCT: {
				value kwrd;
				if (!interpreter_pop(inter, &kwrd)) goto FAILURE_STACK;

				rbt_node* node = NULL;
				node = rbt_search(inter->global_words, kwrd.u);
				if (node) goto FAILURE_REDEFINE;
				node = rbt_node_new(kwrd.u);
				if (!node) goto FAILURE_DEFINE;

				node->data = inter->struct_vector.size;
				node->type = KWRD_STRUCT;
				rbt_insert(inter->global_words, node);

				vector(uint64_t)* temp_struct = (vector(uint64_t)*) malloc(sizeof(vector(uint64_t)));
				if (!temp_struct) goto FAILURE_STRUCT;
				vector_init(uint64_t, temp_struct);
				if (!vector_push_back(cctl_ptr(vector(uint64_t)), &inter->struct_vector, temp_struct)) goto FAILURE_STRUCT;
			} break;
			case OP_MEMBER: {
				value kwrd;
				if (!interpreter_pop(inter, &kwrd)) goto FAILURE_STACK;

				uint64_t last = inter->struct_vector.size - 1;
				vector(uint64_t)* temp_struct = *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, last);
				if (!temp_struct) goto FAILURE_STRUCT;
				for (size_t i = 0; i < temp_struct->size; i++) {
					if (kwrd.u == *vector_at(uint64_t, temp_struct, i)) goto FAILURE_STRUCT;
				}
				if (!vector_push_back(uint64_t, temp_struct, kwrd.u)) goto FAILURE_STRUCT;
			} break;
			case OP_END_STRUCT: {
				
			} break;
			case OP_CALL_MEMBER: {
				value kwrd_struct;
				value kwrd_member;
				value addr;
				rbt_node* node = NULL;

				if (!interpreter_pop(inter, &kwrd_member)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &kwrd_struct)) goto FAILURE_STACK;

				node = rbt_search(inter->global_words, kwrd_struct.u);
				if (!node) goto FAILURE_STRUCT;
				if (node->type != KWRD_STRUCT) goto FAILURE_STRUCT;

				vector(uint64_t)* temp_struct = *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, node->data);
				if (!temp_struct) goto FAILURE_STRUCT;

				uint64_t i;
				bool check = true;
				for (i = 0; i < temp_struct->size; i++) {
					if (kwrd_member.u == *vector_at(uint64_t, temp_struct, i)) {
						check = false;
						break;
					}
				}
				if (check) {
					goto FAILURE_STRUCT;
				}
				if (!interpreter_pop(inter, &addr)) goto FAILURE_STACK;
				addr.u += (i * 8);
				if (!interpreter_push(inter, addr)) goto FAILURE_STACK;
			} break;
			case OP_TO: {
				value kwrd;
				value v;
				rbt* words = NULL;
				rbt_node* node = NULL;
				if (!interpreter_pop(inter, &kwrd)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;

				node = rbt_search(inter->global_words, kwrd.u);
				if (!node) {
					if (inter->local_words_stack.size > 0) {
						words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
						node = rbt_search(words, kwrd.u);
					}
					else {
						words = inter->global_words;
					}
				}
				if (!node) {
					node = rbt_node_new(kwrd.u);
					if (!node) goto FAILURE_DEFINE;
					node->type = KWRD_VAR;
					rbt_insert(words, node);
				}

				if (node->type == KWRD_VAR) {
					node->data = v.u;
				}
				else goto FAILURE_INVALID;
			} break;
			case OP_CALL: {
				value kwrd;
				rbt* local_words = NULL;
				rbt_node* node = NULL;

				if (!interpreter_pop(inter, &kwrd)) goto FAILURE_STACK;

				node = rbt_search(inter->global_words, kwrd.u);
				if (!node) {
					if (inter->local_words_stack.size > 0) {
						local_words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
						node = rbt_search(local_words, kwrd.u);
					}
				}

				if (!node) goto FAILURE_UNDEFINED;

				switch (node->type) {
					case KWRD_FUNC: {
						if (!deque_push_back(size_t, &inter->call_stack, index + 1)) goto FAILURE_CALL;
						local_words = rbt_new();
						if (!local_words) goto FAILURE_CALL;
						if (!deque_push_back(cctl_ptr(rbt), &inter->local_words_stack, local_words)) goto FAILURE_CALL;
						index = node->data - 1;
					} break;
					case KWRD_MACRO: {
						if (!deque_push_back(size_t, &inter->call_stack, index + 1)) goto FAILURE_CALL;
						index = node->data - 1;
					} break;
					case KWRD_VAR: {
						value v;
						v.u = node->data;
						if (!interpreter_push(inter, v)) goto FAILURE_STACK;
					} break;
					case KWRD_STRUCT: {
						value v;
						vector(uint64_t)* temp_struct = *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, node->data);
						if (!temp_struct) goto FAILURE_STRUCT;
						v.u = temp_struct->size;
						if (!interpreter_push(inter, v)) goto FAILURE_STACK;
					} break;
				}
			} break;
			case OP_ADD: {
				value a, b;
				
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.i = a.i + b.i;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_SUB: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.i = a.i - b.i;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_MUL: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.i = a.i * b.i;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_DIV: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!b.i) {
					fputs("error : Division by zero\n", stderr);
				}
				a.i = a.i / b.i;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_MOD: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!b.i) {
					fputs("error : Division by zero\n", stderr);
				}
				a.i = a.i % b.i;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_UDIV: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!b.u) {
					fputs("error : Division by zero\n", stderr);
				}
				a.u = a.u / b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_UMOD: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!b.u) {
					fputs("error : Division by zero\n", stderr);
				}
				a.u = a.u % b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_NEG: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.u = -v.u;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_INC: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.i++;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_DEC: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.i--;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_EQU: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.i == b.i) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_NEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.i != b.i) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_GRT: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.i > b.i) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_GEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.i >= b.i) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_LST: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.i < b.i) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_LEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.i <= b.i) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_UGRT: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.u > b.u) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_UGEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.u >= b.u) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_ULST: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.u < b.u) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_ULEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.u <= b.u) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FADD: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.f = a.f + b.f;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FSUB: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.f = a.f - b.f;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FMUL: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.f = a.f * b.f;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FDIV: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (b.f == 0) {
					fputs("error : Division by zero\n", stderr);
				}
				a.f = a.f / b.f;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FMOD: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (b.f == 0) {
					fputs("error : Division by zero\n", stderr);
				}
				a.f = fmod(a.f, b.f);
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FNEG: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.f = -v.f;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_FEQU: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.f == b.f) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FNEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.f != b.f) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FGRT: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.f > b.f) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FGEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.f >= b.f) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FLST: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.f < b.f) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_FLEQ: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = (a.f <= b.f) ? -1 : 0;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_AND: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = a.u & b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_OR: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = a.u | b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_XOR: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = a.u ^ b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_NOT: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.u = ~v.u;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_LSFT: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = a.u << b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_RSFT: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				a.u = a.u >> b.u;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_DROP: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
			} break;
			case OP_NIP: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_DUP: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_OVER: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_TUCK: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
			} break;
			case OP_SWAP: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_ROT: {
				value a, b, c;
				if (!interpreter_pop(inter, &c)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
			} break;
			case OP_TDROP: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
			} break;
			case OP_TNIP: {
				value a, b, c;
				if (!interpreter_pop(inter, &c)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
			} break;
			case OP_TDUP: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
			} break;
			case OP_TOVER: {
				value a, b, c, d;
				if (!interpreter_pop(inter, &d)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &c)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
				if (!interpreter_push(inter, d)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
			} break;
			case OP_TTUCK: {
				value a, b, c, d;
				if (!interpreter_pop(inter, &d)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &c)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
				if (!interpreter_push(inter, d)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
				if (!interpreter_push(inter, d)) goto FAILURE_STACK;
			} break;
			case OP_TSWAP: {
				value a, b, c, d;
				if (!interpreter_pop(inter, &d)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &c)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
				if (!interpreter_push(inter, d)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
			} break;
			case OP_TROT: {
				value a, b, c, d, e, f;
				if (!interpreter_pop(inter, &f)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &e)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &d)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &c)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, c)) goto FAILURE_STACK;
				if (!interpreter_push(inter, d)) goto FAILURE_STACK;
				if (!interpreter_push(inter, e)) goto FAILURE_STACK;
				if (!interpreter_push(inter, f)) goto FAILURE_STACK;
				if (!interpreter_push(inter, a)) goto FAILURE_STACK;
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
			} break;
			case OP_ALLOC: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				if (!v.u) v.p = NULL;
				else v.p = malloc(v.u * sizeof(value));
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_RESIZE: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				if (!a.u) {
					free(b.p);
					b.p = NULL;
				}
				else b.p = realloc((void*) b.p, a.u * sizeof(value));
				if (!interpreter_push(inter, b)) goto FAILURE_STACK;
			} break;
			case OP_FREE: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				free(v.p);
			} break;
			case OP_FETCH: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.u = *v.p;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_STORE: {
				value a, b;
				if (!interpreter_pop(inter, &b)) goto FAILURE_STACK;
				if (!interpreter_pop(inter, &a)) goto FAILURE_STACK;
				*b.p = a.u;
			} break;
			case OP_STOF: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.f = (double) v.i;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_UTOF: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.f = (double) v.u;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_FTOS: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.i = (int64_t) v.f;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_FTOU: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				v.u = (uint64_t) v.f;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_GETI: {
				value v;
			#if defined(_WIN32)
				if (wscanf(L"%" SCNd64, &(v.i)) != 1) goto FAILURE_STDIN;
			#else
				if (scanf("%" SCNd64, &(v.i)) != 1) goto FAILURE_STDIN;
			#endif
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_GETU: {
				value v;
			#if defined(_WIN32)
				if (wscanf(L"%" SCNu64, &(v.u)) != 1) goto FAILURE_STDIN;
			#else
				if (scanf("%" SCNu64, &(v.u)) != 1) goto FAILURE_STDIN;
			#endif
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_GETF: {
				value v;
			#if defined(_WIN32)
				if (wscanf(L"%lf", &(v.f)) != 1) goto FAILURE_STDIN;
			#else
				if (scanf("%lf", &(v.f)) != 1) goto FAILURE_STDIN;
			#endif
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;
			} break;
			case OP_GETS: {
				value v;
				deque(value) value_reverser;
				deque_init(value, &value_reverser);
			#if defined(_WIN32)
				wint_t result;
				wint_t next;
				size_t count = 0;
				while (true) {
					result = fgetwc(stdin);
					if (result == WEOF) goto FAILURE_STDIN;
					if (result == 10) {
						if (count > 0) break;
						else continue;
					}
					if (!is_surrogate(result)) {
						v.u = result;
					}
					else {
						next = fgetwc(stdin);
						if (next == WEOF) goto FAILURE_STDIN;
						if (is_high_surrogate(result) && is_low_surrogate(next)) {
							v.u = surrogates_to_utf32(result, next);
						}
						else goto FAILURE_STDIN;
					}
					count++;
					if (!deque_push_back(value, &value_reverser, v)) goto FAILURE_STDIN;
				}
			#else
				char* line = NULL;
				char* temp;
				size_t len;
				size_t rc;
				ssize_t read;
				char32_t out;

				size_t count = 0;
				bool empty_line;
				
				while (true) {
					empty_line = false;
					read = getline(&line, &len, stdin);
					if (read == -1) goto FAILURE_STDIN;
					temp = line;
					while (true) {
						rc = mbrtoc32(&out, line, len, &(inter->convert_state));
						if (!rc) break;
						if ((rc > ((size_t) -4)) || (rc == 0)) goto FAILURE_STDIN;
						if (out == 10) {
							if (count == 0) empty_line = true;
							break;
						}
						len -= rc;
						line += rc;
						v.u = out;
						count++;
						if (!deque_push_back(value, &value_reverser, v)) goto FAILURE_STDIN;
					}
					if (empty_line) continue;
					break;
				}

				free(temp);
				
			#endif
				for (size_t i = value_reverser.size - 1; i < -1; i--) {
					v = *deque_at(value, &value_reverser, i);
					if (!interpreter_push(inter, v)) goto FAILURE_STACK;
				}

				v.u = value_reverser.size;
				if (!interpreter_push(inter, v)) goto FAILURE_STACK;

				deque_free(value, &value_reverser);
			} break;
			case OP_PUTC: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				if (v.u < 127) {
					putchar(v.u);
				}
				else {
					char out[8];
					size_t rc = c32rtomb(out, (char32_t) v.u, &(inter->convert_state));
					if (rc == -1) {
						fputs("error : Unicode encoding failure\n", stderr);
						return false; 
					}
					out[rc] = 0;
					fputs(out, stdout);
				}
			} break;
			case OP_PUTI: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				printf("%" PRId64 " ", v.i);
			} break;
			case OP_PUTU: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				printf("%" PRIu64 " ", v.u);
			} break;
			case OP_PUTF: {
				value v;
				if (!interpreter_pop(inter, &v)) goto FAILURE_STACK;
				printf("%lf ", v.f);
			} break;
			case OP_SHOW: {
				printf("[%zu] [ ", inter->data_stack.size);
				for (size_t i = 0; i < inter->data_stack.size; i++) {
					printf("%" PRId64 " ", (*deque_at(value, &inter->data_stack, i)).i);
				}
				printf("]\n");
			} break;
			default: {
				fprintf(stderr, "\'%u\'\n", *code);
				fputs("error : Invalid operation code\n", stderr);
				return false;
			}
		}
	}
	return true;

FAILURE_STACK:
	fputs("error : Stack memory error\n", stderr);
	return false;
FAILURE_INVALID:
	fputs("error : Invalid keyword\n", stderr);
	return false;
FAILURE_UNDEFINED:
	fputs("error : Undefined keyword\n", stderr);
	return false;
FAILURE_REDEFINE:
	fputs("error : Redefined keyword\n", stderr);
	return false;
FAILURE_DEFINE:
	fputs("error : Definition failure\n", stderr);
	return false;
FAILURE_CALL:
	fputs("error : Call stack error\n", stderr);
	return false;
FAILURE_SWITCH:
	fputs("error : Switch stack error\n", stderr);
	return false;
FAILURE_STRUCT:
	fputs("error : Struct definition failure", stderr);
	return false;
FAILURE_STDIN:
	fputs("error: Input error\n", stderr);
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