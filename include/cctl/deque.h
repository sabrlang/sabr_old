#ifndef __CCTL_DEQUE_H__
#define __CCTL_DEQUE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cctl.h"

#define chunk(TYPE) cctl_join(TYPE, chunk)
#define deque(TYPE) cctl_join(TYPE, deque)
#define chunk_func(FUNC, TYPE) cctl_join(chunk(TYPE), FUNC)
#define deque_chunk_func(FUNC, TYPE) cctl_join(cctl_join(deque(TYPE), chunk), FUNC)
#define deque_func(FUNC, TYPE) cctl_join(deque(TYPE), FUNC)
#define deque_struct(TYPE) cctl_join(deque(TYPE), struct)

#define deque_init(TYPE, p_v) deque_func(init, TYPE)(p_v)
#define deque_resize(TYPE, p_v, size) deque_func(resize, TYPE)(p_v, size)
#define deque_reserve(TYPE, p_v, size) deque_func(reserve, TYPE)(p_v, size)
#define deque_clear(TYPE, p_v) deque_func(clear, TYPE)(p_v)
#define deque_free(TYPE, p_v) deque_func(free, TYPE)(p_v)
#define deque_push_back(TYPE, p_v, item) deque_func(push_back, TYPE)(p_v, item)
#define deque_pop_back(TYPE, p_v) deque_func(pop_back, TYPE)(p_v)
#define deque_at(TYPE, p_v, index) deque_func(at, TYPE)(p_v, index)
#define deque_front(TYPE, p_v) deque_func(front, TYPE)(p_v)
#define deque_back(TYPE, p_v) deque_func(back, TYPE)(p_v)

#define deque_fd(TYPE) \
	typedef struct chunk_struct(TYPE) chunk(TYPE); \
	typedef struct deque_struct(TYPE) deque(TYPE);

#define deque_imp_h(TYPE) \
	struct chunk_struct(TYPE) { \
		TYPE* p_data; \
		size_t begin; \
		size_t end; \
		size_t size; \
		size_t capacity; \
		bool allocated; \
	} \
	\
	struct deque_struct(TYPE) { \
		chunk(TYPE)* p_data; \
		size_t chunk_begin; \
		size_t chunk_end; \
		size_t chunk_count; \
		size_t chunk_capacity; \
		bool allocated; \
		size_t size; \
	}; \
	bool deque_func(reserve, TYPE)(deque(TYPE)* p_v, size_t size); \
	bool deque_func(resize, TYPE)(deque(TYPE)* p_v, size_t size); \
	void deque_func(init, TYPE)(deque(TYPE)* p_v); \
	void deque_func(clear, TYPE)(deque(TYPE)* p_v); \
	void deque_func(free, TYPE)(deque(TYPE)* p_v); \
	bool deque_func(push_back, TYPE)(deque(TYPE)* p_v, TYPE item); \
	bool deque_func(pop_back, TYPE)(deque(TYPE)* p_v); \
	TYPE* deque_func(at, TYPE)(deque(TYPE)* p_v, size_t index); \
	TYPE* deque_func(front, TYPE)(deque(TYPE)* p_v); \
	TYPE* deque_func(back, TYPE)(deque(TYPE)* p_v);



#endif