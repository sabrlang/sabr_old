#ifndef __CCTL_LIST_H__
#define __CCTL_LIST_H__

#include <stdbool.h>
#include <stdlib.h>

#include "cctl.h"

#define node(TYPE) cctl_join(TYPE, node)
#define node_func(FUNC, TYPE) cctl_join(node(TYPE), FUNC)
#define node_struct(TYPE) cctl_join(node(TYPE), struct)

#define node_imp_h(TYPE) \
	typedef struct node_struct(TYPE) node(TYPE); \
	\
	struct node_struct(TYPE) { \
		TYPE data; \
		node(TYPE)* p_prev; \
		node(TYPE)* p_next; \
	}; \
	\
	void node_func(init, TYPE)(node(TYPE)* p_node);

#define node_imp_c(TYPE) \
	void node_func(init, TYPE)(node(TYPE)* p_node) { \
		p_node->p_prev = p_node->p_next = NULL; \
	}

#define list(TYPE) cctl_join(TYPE, list)
#define list_func(FUNC, TYPE) cctl_join(list(TYPE), FUNC)
#define list_struct(TYPE) cctl_join(list(TYPE), struct)

#define list_init(TYPE, p_l) list_func(init, TYPE)(p_l)
#define list_free(TYPE, p_l) list_func(free, TYPE)(p_l)
#define list_push_front(TYPE, p_l, item) list_func(push_front, TYPE)(p_l, item)
#define list_push_back(TYPE, p_l, item) list_func(push_back, TYPE)(p_l, item)
#define list_pop_front(TYPE, p_l) list_func(pop_front, TYPE)(p_l)
#define list_pop_back(TYPE, p_l) list_func(pop_back, TYPE)(p_l)
#define list_at(TYPE, p_l, index) list_func(at, TYPE)(p_l, index)

#define list_node_front(TYPE, p_l) ((p_l)->p_front)
#define list_node_back(TYPE, p_l) ((p_l)->p_back)
#define list_front(TYPE, p_l) (&(list_node_front(TYPE, p_l)->data))
#define list_back(TYPE, p_l) (&(list_node_back(TYPE, p_l)->data))

#define list_fd(TYPE) \
	typedef struct list_struct(TYPE) list(TYPE);

#define list_imp_h(TYPE) \
	node_imp_h(TYPE); \
	\
	struct list_struct(TYPE) { \
		node(TYPE)* p_front; \
		node(TYPE)* p_back; \
		size_t size; \
	}; \
	\
	void list_func(init, TYPE)(list(TYPE)* p_l); \
	void list_func(free, TYPE)(list(TYPE)* p_l); \
	bool list_func(push_front, TYPE)(list(TYPE)* p_l, TYPE item); \
	bool list_func(push_back, TYPE)(list(TYPE)* p_l, TYPE item); \
	void list_func(pop_back, TYPE)(list(TYPE)* p_l); \
	void list_func(pop_front, TYPE)(list(TYPE)* p_l); \
	TYPE* list_func(at, TYPE)(list(TYPE)* p_l, size_t index);

#define list_imp_c(TYPE) \
	node_imp_c(TYPE) \
	\
	void list_func(init, TYPE)(list(TYPE)* p_l) { \
		p_l->p_front = p_l->p_back = NULL; \
		p_l->size = 0; \
	} \
	\
	void list_func(free, TYPE)(list(TYPE)* p_l) { \
		while (p_l->size) { \
			list_func(pop_back, TYPE)(p_l); \
		} \
	} \
	\
	bool list_func(push_front, TYPE)(list(TYPE)* p_l, TYPE item) { \
		node(TYPE)* p_node = NULL; \
		if (!(p_node = ((node(TYPE)*) malloc(sizeof(node(TYPE)))))) return false; \
		node_func(init, TYPE)(p_node); \
		p_node->data = item; \
		if (p_l->p_front) { \
			p_node->p_next = p_l->p_front; \
			p_l->p_front->p_prev = p_node; \
			p_l->p_front = p_node; \
		} \
		else p_l->p_back = p_l->p_front = p_node; \
		p_l->size++; \
		return true; \
	} \
	\
	bool list_func(push_back, TYPE)(list(TYPE)* p_l, TYPE item) { \
		node(TYPE)* p_node = NULL; \
		if (!(p_node = ((node(TYPE)*) malloc(sizeof(node(TYPE)))))) return false; \
		node_func(init, TYPE)(p_node); \
		p_node->data = item; \
		if (p_l->p_back) { \
			p_node->p_prev = p_l->p_back; \
			p_l->p_back->p_next = p_node; \
			p_l->p_back = p_node; \
		} \
		else p_l->p_back = p_l->p_front = p_node; \
		p_l->size++; \
		return true; \
	} \
	\
	void list_func(pop_front, TYPE)(list(TYPE)* p_l) { \
		if (p_l->size) { \
			node(TYPE)* p_node = p_l->p_front; \
			p_l->p_front = p_node->p_next; \
			if (p_l->p_front) \
				p_l->p_front->p_prev = NULL; \
			else \
				p_l->p_back = NULL; \
			p_l->size--; \
			free(p_node); \
		} \
	} \
	\
	void list_func(pop_back, TYPE)(list(TYPE)* p_l) { \
		if (p_l->size) { \
			node(TYPE)* p_node = p_l->p_back; \
			p_l->p_back = p_node->p_prev; \
			if (p_l->p_back) \
				p_l->p_back->p_next = NULL; \
			else \
				p_l->p_front = NULL; \
			p_l->size--; \
			free(p_node); \
		} \
	} \
	\
	TYPE* list_func(at, TYPE)(list(TYPE)* p_l, size_t index) { \
		bool direction = (index > (p_l->size / 2)); \
		node(TYPE)* temp_node = direction ? list_node_back(TYPE, p_l) : list_node_front(TYPE, p_l); \
		if (index == 0 || index == (p_l->size -1)) return &(temp_node->data); \
		size_t temp_index = direction ? (p_l->size - 1) : 0; \
		do { \
			if (direction) { \
				temp_index -= 1; \
				temp_node = temp_node->p_prev; \
			} \
			else { \
				temp_index += 1; \
				temp_node = temp_node->p_next; \
			} \
		} while (temp_index != index); \
		return &(temp_node->data); \
	}
	
#endif