#include "interpreter_op.h"

uint32_t interpreter_op_exit(interpreter* inter, size_t* index) {
	*index = inter->bytecode_size;
	return OPERR_NONE;
}

uint32_t interpreter_op_value(interpreter* inter, size_t* index) {
	value v;
	for (int i = 0; i < 8; i++) {
		v.bytes[i] = inter->bytecode[++(*index)];
	}
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_if(interpreter* inter, size_t* index) {
	value pos;
	value v;
	for (int i = 0; i < 8; i++) {
		pos.bytes[i] = inter->bytecode[++(*index)];
	}
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	if (!v.u) *index = pos.u - 1;

	return OPERR_NONE;
}

uint32_t interpreter_op_jump(interpreter* inter, size_t* index) {
	value pos;
	for (int i = 0; i < 8; i++) {
		pos.bytes[i] = inter->bytecode[++(*index)];
	}
	*index = pos.u - 1;

	return OPERR_NONE;
}

uint32_t interpreter_op_switch(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	if (!deque_push_back(value, &inter->switch_stack, v)) return OPERR_SWITCH;

	return OPERR_NONE;
}

uint32_t interpreter_op_case(interpreter* inter, size_t* index) {
	value v;
	v = *deque_back(value, &inter->switch_stack);
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_end_switch(interpreter* inter, size_t* index) {
	if (!deque_pop_back(value, &inter->switch_stack)) return OPERR_SWITCH;

	return OPERR_NONE;
}

uint32_t interpreter_op_func(interpreter* inter, size_t* index) {
	value kwrd;
	value pos;

	if (!interpreter_pop(inter, &kwrd)) return OPERR_STACK;
	rbt_node* node = NULL;
	node = rbt_search(inter->global_words, kwrd.u);
	if (node) return OPERR_REDEFINE;
	node = rbt_node_new(kwrd.u);
	if (!node) return OPERR_DEFINE;

	node->data = *index + 9;
	node->type = KWRD_FUNC;
	rbt_insert(inter->global_words, node);

	for (int i = 0; i < 8; i++) {
		pos.bytes[i] = inter->bytecode[++(*index)];
	}
	*index = pos.u - 1;

	return OPERR_NONE;
}

uint32_t interpreter_op_macro(interpreter* inter, size_t* index) {
	value kwrd;
	value pos;
	if (!interpreter_pop(inter, &kwrd)) return OPERR_STACK;
	rbt_node* node = NULL;
	node = rbt_search(inter->global_words, kwrd.u);
	if (node) return OPERR_REDEFINE;
	node = rbt_node_new(kwrd.u);
	if (!node) return OPERR_DEFINE;

	node->data = *index + 9;
	node->type = KWRD_MACRO;
	rbt_insert(inter->global_words, node);

	for (int i = 0; i < 8; i++) {
		pos.bytes[i] = inter->bytecode[++(*index)];
	}
	*index = pos.u - 1;

	return OPERR_NONE;
}

uint32_t interpreter_op_return_func(interpreter* inter, size_t* index) {
	if (inter->call_stack.size < 1) return OPERR_CALL;
	size_t pos = *deque_back(size_t, &inter->call_stack);
	if (!deque_pop_back(size_t, &inter->call_stack)) return OPERR_CALL;
	rbt* local_words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
	rbt_free(local_words);
	if (!deque_pop_back(cctl_ptr(rbt), &inter->local_words_stack)) return OPERR_CALL;
	*index = pos - 1;

	size_t* local_memory_size = deque_back(size_t, &inter->local_memory_size_stack);
	if (!local_memory_size) return OPERR_CALL;
	if (!interpreter_mem_free(inter, *local_memory_size)) return OPERR_CALL;
	if (!deque_pop_back(size_t, &inter->local_memory_size_stack)) return OPERR_CALL;

	return OPERR_NONE;
}

uint32_t interpreter_op_return_macro(interpreter* inter, size_t* index) {
	if (inter->call_stack.size < 1) return OPERR_CALL;
	size_t pos = *deque_back(size_t, &inter->call_stack);
	if (!deque_pop_back(size_t, &inter->call_stack)) return OPERR_CALL;
	*index = pos - 1;

	return OPERR_NONE;
}

uint32_t interpreter_op_struct(interpreter* inter, size_t* index) {
	value kwrd;
	if (!interpreter_pop(inter, &kwrd)) return OPERR_STACK;

	rbt_node* node = NULL;
	node = rbt_search(inter->global_words, kwrd.u);
	if (node) return OPERR_REDEFINE;
	node = rbt_node_new(kwrd.u);
	if (!node) return OPERR_DEFINE;

	node->data = inter->struct_vector.size;
	node->type = KWRD_STRUCT;
	rbt_insert(inter->global_words, node);

	vector(uint64_t)* temp_struct = (vector(uint64_t)*) malloc(sizeof(vector(uint64_t)));
	if (!temp_struct) return OPERR_STRUCT;
	vector_init(uint64_t, temp_struct);
	if (!vector_push_back(cctl_ptr(vector(uint64_t)), &inter->struct_vector, temp_struct)) return OPERR_STRUCT;

	return OPERR_NONE;
}

uint32_t interpreter_op_member(interpreter* inter, size_t* index) {
	value kwrd;
	if (!interpreter_pop(inter, &kwrd)) return OPERR_STACK;

	uint64_t last = inter->struct_vector.size - 1;
	vector(uint64_t)* temp_struct = *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, last);
	if (!temp_struct) return OPERR_STRUCT;
	for (size_t i = 0; i < temp_struct->size; i++) {
		if (kwrd.u == *vector_at(uint64_t, temp_struct, i)) return OPERR_STRUCT;
	}
	if (!vector_push_back(uint64_t, temp_struct, kwrd.u)) return OPERR_STRUCT;

	return OPERR_NONE;
}

uint32_t interpreter_op_end_struct(interpreter* inter, size_t* index) {
	return OPERR_NONE;
}

uint32_t interpreter_op_call_member(interpreter* inter, size_t* index) {
	value kwrd_struct;
	value kwrd_member;
	value addr;
	rbt_node* node = NULL;

	if (!interpreter_pop(inter, &kwrd_member)) return OPERR_STACK;
	if (!interpreter_pop(inter, &kwrd_struct)) return OPERR_STACK;

	node = rbt_search(inter->global_words, kwrd_struct.u);
	if (!node) return OPERR_STRUCT;
	if (node->type != KWRD_STRUCT) return OPERR_STRUCT;

	vector(uint64_t)* temp_struct = *vector_at(cctl_ptr(vector(uint64_t)), &inter->struct_vector, node->data);
	if (!temp_struct) return OPERR_STRUCT;

	uint64_t i;
	bool check = true;
	for (i = 0; i < temp_struct->size; i++) {
		if (kwrd_member.u == *vector_at(uint64_t, temp_struct, i)) {
			check = false;
			break;
		}
	}
	if (check) {
		return OPERR_STRUCT;
	}
	if (!interpreter_pop(inter, &addr)) return OPERR_STACK;
	addr.u += (i * 8);
	if (!interpreter_push(inter, addr)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_set(interpreter* inter, size_t* index) {
	value kwrd;
	value v;
	rbt* words = NULL;
	rbt_node* node = NULL;
	if (!interpreter_pop(inter, &kwrd)) return OPERR_STACK;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;

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
		if (!node) return OPERR_DEFINE;
		node->type = KWRD_VAR;
		
		value* p = interpreter_mem_top(inter);
		size_t* local_memory_size = deque_back(size_t, &inter->local_memory_size_stack);
		if (!local_memory_size) return OPERR_MEMORY;
		local_memory_size++;
		if (!interpreter_mem_alloc(inter, 1)) return OPERR_MEMORY;
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

uint32_t interpreter_op_call(interpreter* inter, size_t* index) {
	value kwrd;
	rbt* local_words = NULL;
	rbt_node* node = NULL;

	if (!interpreter_pop(inter, &kwrd)) return OPERR_STACK;

	node = rbt_search(inter->global_words, kwrd.u);
	if (!node) {
		if (inter->local_words_stack.size > 0) {
			local_words = *deque_back(cctl_ptr(rbt), &inter->local_words_stack);
			node = rbt_search(local_words, kwrd.u);
		}
	}

	if (!node) return OPERR_UNDEFINED;

	switch (node->type) {
		case KWRD_FUNC: {
			if (!deque_push_back(size_t, &inter->call_stack, *index + 1)) return OPERR_CALL;
			local_words = rbt_new();
			if (!local_words) return OPERR_CALL;
			if (!deque_push_back(cctl_ptr(rbt), &inter->local_words_stack, local_words)) return OPERR_CALL;

			deque_push_back(size_t, &inter->local_memory_size_stack, 0);

			*index = node->data - 1;
		} break;
		case KWRD_MACRO: {
			if (!deque_push_back(size_t, &inter->call_stack, *index + 1)) return OPERR_CALL;
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

uint32_t interpreter_op_add(interpreter* inter, size_t* index) {
	value a, b;
				
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.i = a.i + b.i;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_sub(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.i = a.i - b.i;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_mul(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.i = a.i * b.i;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_div(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!b.i) {
		return OPERR_DIV_BY_ZERO;
	}
	a.i = a.i / b.i;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_mod(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!b.i) {
		return OPERR_DIV_BY_ZERO;
	}
	a.i = a.i % b.i;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_udiv(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!b.u) {
		return OPERR_DIV_BY_ZERO;
	}
	a.u = a.u / b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_umod(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!b.u) {
		return OPERR_DIV_BY_ZERO;
	}
	a.u = a.u % b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_neg(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.u = -v.u;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_inc(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.i++;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_dec(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.i--;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_equ(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.i == b.i) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_neq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.i != b.i) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_grt(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.i > b.i) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_geq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.i >= b.i) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_lst(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.i < b.i) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_leq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.i <= b.i) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_ugrt(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.u > b.u) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_ugeq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.u >= b.u) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_ulst(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.u < b.u) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_uleq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.u <= b.u) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fadd(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.f = a.f + b.f;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fsub(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.f = a.f - b.f;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fmul(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.f = a.f * b.f;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fdiv(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (b.f == 0) {
		return OPERR_DIV_BY_ZERO;
	}
	a.f = a.f / b.f;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fmod(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (b.f == 0) {
		return OPERR_DIV_BY_ZERO;
	}
	a.f = fmod(a.f, b.f);
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fneg(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.f = -v.f;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fequ(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.f == b.f) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fneq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.f != b.f) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fgrt(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.f > b.f) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fgeq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.f >= b.f) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_flst(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.f < b.f) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_fleq(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = (a.f <= b.f) ? -1 : 0;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_and(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = a.u & b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_or(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = a.u | b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_xor(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = a.u ^ b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_not(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.u = ~v.u;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_lsft(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = a.u << b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_rsft(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	a.u = a.u >> b.u;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_drop(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_nip(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_dup(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	if (!interpreter_push(inter, v)) return OPERR_STACK;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_over(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_tuck(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_swap(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_rot(interpreter* inter, size_t* index) {
	value a, b, c;
	if (!interpreter_pop(inter, &c)) return OPERR_STACK;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_tdrop(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_tnip(interpreter* inter, size_t* index) {
	value a, b, c;
	if (!interpreter_pop(inter, &c)) return OPERR_STACK;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_tdup(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	return OPERR_NONE;
}

uint32_t interpreter_op_tover(interpreter* inter, size_t* index) {
	value a, b, c, d;
	if (!interpreter_pop(inter, &d)) return OPERR_STACK;
	if (!interpreter_pop(inter, &c)) return OPERR_STACK;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;
	if (!interpreter_push(inter, d)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	return OPERR_NONE;
}

uint32_t interpreter_op_ttuck(interpreter* inter, size_t* index) {
	value a, b, c, d;
	if (!interpreter_pop(inter, &d)) return OPERR_STACK;
	if (!interpreter_pop(inter, &c)) return OPERR_STACK;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;
	if (!interpreter_push(inter, d)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;
	if (!interpreter_push(inter, d)) return OPERR_STACK;
	return OPERR_NONE;
}

uint32_t interpreter_op_tswap(interpreter* inter, size_t* index) {
	value a, b, c, d;
	if (!interpreter_pop(inter, &d)) return OPERR_STACK;
	if (!interpreter_pop(inter, &c)) return OPERR_STACK;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;
	if (!interpreter_push(inter, d)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_trot(interpreter* inter, size_t* index) {
	value a, b, c, d, e, f;
	if (!interpreter_pop(inter, &f)) return OPERR_STACK;
	if (!interpreter_pop(inter, &e)) return OPERR_STACK;
	if (!interpreter_pop(inter, &d)) return OPERR_STACK;
	if (!interpreter_pop(inter, &c)) return OPERR_STACK;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!interpreter_push(inter, c)) return OPERR_STACK;
	if (!interpreter_push(inter, d)) return OPERR_STACK;
	if (!interpreter_push(inter, e)) return OPERR_STACK;
	if (!interpreter_push(inter, f)) return OPERR_STACK;
	if (!interpreter_push(inter, a)) return OPERR_STACK;
	if (!interpreter_push(inter, b)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_alloc(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	if (!v.u) v.p = NULL;
	else v.p = malloc(v.u);
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_resize(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	if (!a.u) {
		free(b.p);
		b.p = NULL;
	}
	else b.p = realloc((void*) b.p, a.u);
	if (!interpreter_push(inter, b)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_free(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	free(v.p);

	return OPERR_NONE;
}

uint32_t interpreter_op_allot(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;

	value* p = interpreter_mem_top(inter);

	size_t* local_memory_size = deque_back(size_t, &inter->local_memory_size_stack);
	if (!local_memory_size) return OPERR_MEMORY;
	local_memory_size += v.u / sizeof(value);

	if (!interpreter_mem_alloc(inter, v.u / sizeof(value))) return OPERR_MEMORY;
	v.p = (uint64_t*) p;

	if (!interpreter_push(inter, v)) return OPERR_STACK;
	return OPERR_NONE;
}

uint32_t interpreter_op_fetch(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.u = *v.p;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_store(interpreter* inter, size_t* index) {
	value a, b;
	if (!interpreter_pop(inter, &b)) return OPERR_STACK;
	if (!interpreter_pop(inter, &a)) return OPERR_STACK;
	*b.p = a.u;

	return OPERR_NONE;
}

uint32_t interpreter_op_stof(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.f = (double) v.i;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_utof(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.f = (double) v.u;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_ftos(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.i = (int64_t) v.f;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_ftou(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	v.u = (uint64_t) v.f;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_geti(interpreter* inter, size_t* index) {
	value v;
#if defined(_WIN32)
	if (wscanf(L"%" SCNd64, &(v.i)) != 1) return OPERR_STDIN;
#else
	if (scanf("%" SCNd64, &(v.i)) != 1) return OPERR_STDIN;
#endif
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_getu(interpreter* inter, size_t* index) {
	value v;
#if defined(_WIN32)
	if (wscanf(L"%" SCNu64, &(v.u)) != 1) return OPERR_STDIN;
#else
	if (scanf("%" SCNu64, &(v.u)) != 1) return OPERR_STDIN;
#endif
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_getf(interpreter* inter, size_t* index) {
	value v;
#if defined(_WIN32)
	if (wscanf(L"%lf", &(v.f)) != 1) return OPERR_STDIN;
#else
	if (scanf("%lf", &(v.f)) != 1) return OPERR_STDIN;
#endif
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	return OPERR_NONE;
}

uint32_t interpreter_op_gets(interpreter* inter, size_t* index) {
	value v;
	deque(value) value_reverser;
	deque_init(value, &value_reverser);
#if defined(_WIN32)
	wint_t result;
	wint_t next;
	size_t count = 0;
	while (true) {
		result = fgetwc(stdin);
		if (result == WEOF) return OPERR_STDIN;
		if (result == 10) {
			if (count > 0) break;
			else continue;
		}
		if (!is_surrogate(result)) {
			v.u = result;
		}
		else {
			next = fgetwc(stdin);
			if (next == WEOF) return OPERR_STDIN;
			if (is_high_surrogate(result) && is_low_surrogate(next)) {
				v.u = surrogates_to_utf32(result, next);
			}
			else return OPERR_STDIN;
		}
		count++;
		if (!deque_push_back(value, &value_reverser, v)) return OPERR_STDIN;
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
		if (read == -1) return OPERR_STDIN;
		temp = line;
		while (true) {
			rc = mbrtoc32(&out, line, len, &(inter->convert_state));
			if (!rc) break;
			if ((rc > ((size_t) -4)) || (rc == 0)) return OPERR_STDIN;
			if (out == 10) {
				if (count == 0) empty_line = true;
				break;
			}
			len -= rc;
			line += rc;
			v.u = out;
			count++;
			if (!deque_push_back(value, &value_reverser, v)) return OPERR_STDIN;
		}
		if (empty_line) continue;
		break;
	}

	free(temp);
	
#endif
	for (size_t i = value_reverser.size - 1; i < -1; i--) {
		v = *deque_at(value, &value_reverser, i);
		if (!interpreter_push(inter, v)) return OPERR_STACK;
	}

	v.u = value_reverser.size;
	if (!interpreter_push(inter, v)) return OPERR_STACK;

	deque_free(value, &value_reverser);

	return OPERR_NONE;
}

uint32_t interpreter_op_putc(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	if (v.u < 127) {
		putchar(v.u);
	}
	else {
		char out[8];
		size_t rc = c32rtomb(out, (char32_t) v.u, &(inter->convert_state));
		if (rc == -1) {
			return OPERR_UNICODE;
		}
		out[rc] = 0;
		fputs(out, stdout);
	}

	return OPERR_NONE;
}

uint32_t interpreter_op_puti(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	printf("%" PRId64 " ", v.i);

	return OPERR_NONE;
}

uint32_t interpreter_op_putu(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	printf("%" PRIu64 " ", v.u);

	return OPERR_NONE;
}

uint32_t interpreter_op_putf(interpreter* inter, size_t* index) {
	value v;
	if (!interpreter_pop(inter, &v)) return OPERR_STACK;
	printf("%lf ", v.f);

	return OPERR_NONE;
}

uint32_t interpreter_op_show(interpreter* inter, size_t* index) {
	printf("[%zu] [ ", inter->data_stack.size);
	for (size_t i = 0; i < inter->data_stack.size; i++) {
		printf("%" PRId64 " ", (*deque_at(value, &inter->data_stack, i)).i);
	}
	printf("]\n");

	return OPERR_NONE;
}

const uint32_t (*interpreter_op_functions[])(interpreter*, size_t*) = {
	interpreter_op_exit,
	interpreter_op_value,
	interpreter_op_if,
	interpreter_op_jump,
	interpreter_op_switch,
	interpreter_op_case,
	interpreter_op_end_switch,
	interpreter_op_func,
	interpreter_op_macro,
	interpreter_op_return_func,
	interpreter_op_return_macro,
	interpreter_op_struct,
	interpreter_op_member,
	interpreter_op_end_struct,
	interpreter_op_call_member,
	interpreter_op_set,
	interpreter_op_call,
	interpreter_op_add,
	interpreter_op_sub,
	interpreter_op_mul,
	interpreter_op_div,
	interpreter_op_mod,
	interpreter_op_udiv,
	interpreter_op_umod,
	interpreter_op_neg,
	interpreter_op_inc,
	interpreter_op_dec,
	interpreter_op_equ,
	interpreter_op_neq,
	interpreter_op_grt,
	interpreter_op_geq,
	interpreter_op_lst,
	interpreter_op_leq,
	interpreter_op_ugrt,
	interpreter_op_ugeq,
	interpreter_op_ulst,
	interpreter_op_uleq,
	interpreter_op_fadd,
	interpreter_op_fsub,
	interpreter_op_fmul,
	interpreter_op_fdiv,
	interpreter_op_fmod,
	interpreter_op_fneg,
	interpreter_op_fequ,
	interpreter_op_fneq,
	interpreter_op_fgrt,
	interpreter_op_fgeq,
	interpreter_op_flst,
	interpreter_op_fleq,
	interpreter_op_and,
	interpreter_op_or,
	interpreter_op_xor,
	interpreter_op_not,
	interpreter_op_lsft,
	interpreter_op_rsft,
	interpreter_op_drop,
	interpreter_op_nip,
	interpreter_op_dup,
	interpreter_op_over,
	interpreter_op_tuck,
	interpreter_op_swap,
	interpreter_op_rot,
	interpreter_op_tdrop,
	interpreter_op_tnip,
	interpreter_op_tdup,
	interpreter_op_tover,
	interpreter_op_ttuck,
	interpreter_op_tswap,
	interpreter_op_trot,
	interpreter_op_alloc,
	interpreter_op_resize,
	interpreter_op_free,
	interpreter_op_allot,
	interpreter_op_fetch,
	interpreter_op_store,
	interpreter_op_stof,
	interpreter_op_utof,
	interpreter_op_ftos,
	interpreter_op_ftou,
	interpreter_op_geti,
	interpreter_op_getu,
	interpreter_op_getf,
	interpreter_op_gets,
	interpreter_op_putc,
	interpreter_op_puti,
	interpreter_op_putu,
	interpreter_op_putf,
	interpreter_op_show
};

size_t interpreter_op_functions_len = sizeof(interpreter_op_functions) / sizeof(char*);