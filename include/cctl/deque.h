#ifndef __CCTL_DEQUE_H__
#define __CCTL_DEQUE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cctl.h"

#define chunk(TYPE) cctl_join(TYPE, chunk)
#define chunk_func(FUNC, TYPE) cctl_join(chunk(TYPE), FUNC)
#define chunk_struct(TYPE) cctl_join(chunk(TYPE), struct)

#define chunk_fd(TYPE) \
	typedef struct chunk_struct(TYPE) chunk(TYPE); \

#define chunk_imp_h(TYPE) \
	struct chunk_struct(TYPE) { \
		TYPE* p_data; \
		size_t begin; \
		size_t end; \
		size_t size; \
		size_t capacity; \
		bool allocated; \
	}; \
	\
	bool chunk_func(reserve, TYPE)(chunk(TYPE)* p_c, size_t size); \
	void chunk_func(init, TYPE)(chunk(TYPE)* p_c); \
	void chunk_func(clear, TYPE)(chunk(TYPE)* p_c); \
	void chunk_func(free, TYPE)(chunk(TYPE)* p_c); \
	bool chunk_func(push_front, TYPE)(chunk(TYPE)* p_c, TYPE item); \
	bool chunk_func(push_back, TYPE)(chunk(TYPE)* p_c, TYPE item); \
	bool chunk_func(pop_front, TYPE)(chunk(TYPE)* p_c); \
	bool chunk_func(pop_back, TYPE)(chunk(TYPE)* p_c); \
	TYPE* chunk_func(at, TYPE)(chunk(TYPE)* p_c, size_t index); \
	TYPE* chunk_func(front, TYPE)(chunk(TYPE)* p_c); \
	TYPE* chunk_func(back, TYPE)(chunk(TYPE)* p_c);

#define chunk_imp_c(TYPE) \
	bool chunk_func(reserve, TYPE)(chunk(TYPE)* p_c, size_t size) { \
		if (p_c->capacity == size) return true; \
		size_t capacity = 1; \
		if (size > 0) while (capacity < size) capacity <<= 1; \
		if (p_c->capacity < size) { \
			int* p_temp = NULL; \
			if (!(p_temp = (TYPE*) calloc(capacity, sizeof(TYPE)))) return false; \
			if (p_c->allocated) { \
				if (p_c->begin > p_c->end) { \
					size_t temp_size = p_c->capacity - p_c->begin; \
					memcpy(p_temp, p_c->p_data + p_c->begin, temp_size * sizeof(TYPE)); \
					memcpy(p_temp + temp_size, p_c->p_data, (p_c->end + 1) * sizeof(TYPE)); \
				} \
				else memcpy(p_temp, p_c->p_data + p_c->begin, p_c->capacity * sizeof(TYPE)); \
				free(p_c->p_data); \
			} \
			else p_c->allocated = true; \
			p_c->p_data = p_temp; \
			p_c->begin = 0; \
			p_c->end = p_c->size - 1; \
			p_c->capacity = capacity; \
		} \
		return true; \
	} \
	\
	extern inline void chunk_func(init, TYPE)(chunk(TYPE)* p_c) { \
		p_c->p_data = NULL; \
		p_c->begin = 0; \
		p_c->end = -1; \
		p_c->size = 0; \
		p_c->capacity = 0; \
		p_c->allocated = false; \
		return; \
	} \
	\
	extern inline void chunk_func(clear, TYPE)(chunk(TYPE)* p_c) { \
		memset(p_c->p_data, 0, p_c->capacity * sizeof(TYPE)); \
		p_c->begin = 0; \
		p_c->end = -1; \
		p_c->size = 0; \
		return; \
	} \
	\
	extern inline void chunk_func(free, TYPE)(chunk(TYPE)* p_c) { \
		free(p_c->p_data); \
		chunk_func(init, TYPE)(p_c); \
		return; \
	} \
	\
	bool chunk_func(push_front, TYPE)(chunk(TYPE)* p_c, TYPE item) { \
		if (chunk_func(reserve, TYPE)(p_c, p_c->size + 1)) { \
			p_c->size++; \
			if (p_c->begin == 0) p_c->begin = p_c->capacity; \
			p_c->begin--; \
			*(p_c->p_data + p_c->begin) = item; \
			return true; \
		} \
		return false; \
	} \
	\
	bool chunk_func(push_back, TYPE)(chunk(TYPE)* p_c, TYPE item) { \
		if (chunk_func(reserve, TYPE)(p_c, p_c->size + 1)) { \
			p_c->size++; \
			if (p_c->end >= p_c->capacity - 1) p_c->end = -1; \
			p_c->end++; \
			*(p_c->p_data + p_c->end) = item; \
			return true; \
		} \
		return false; \
	} \
	\
	bool chunk_func(pop_front, TYPE)(chunk(TYPE)* p_c) { \
		if (p_c->size == 0) return false; \
		p_c->size--; \
		memset(p_c->p_data + p_c->begin, 0, sizeof(TYPE)); \
		if (p_c->begin == p_c->capacity - 1) p_c->begin = -1; \
		p_c->begin++; \
		return true; \
	} \
	\
	bool chunk_func(pop_back, TYPE)(chunk(TYPE)* p_c) { \
		if (p_c->size == 0) return false; \
		p_c->size--; \
		memset(p_c->p_data + p_c->end, 0, sizeof(TYPE)); \
		if (p_c->end == 0) p_c->end = p_c->capacity; \
		p_c->end--; \
		return true; \
	} \
	\
	TYPE* chunk_func(at, TYPE)(chunk(TYPE)* p_c, size_t index) { \
		if (p_c->begin > p_c->end) { \
			size_t temp = p_c->capacity - 1 - p_c->begin; \
			if (index > temp) \
				return p_c->p_data + index - temp - 1; \
		} \
		return p_c->p_data + p_c->begin + index; \
	} \
	\
	extern inline TYPE* chunk_func(front, TYPE)(chunk(TYPE)* p_c) { \
		return p_c->p_data + p_c->begin; \
	} \
	\
	extern inline TYPE* chunk_func(back, TYPE)(chunk(TYPE)* p_c) { \
		return p_c->p_data + p_c->end; \
	}

#define deque(TYPE) cctl_join(TYPE, deque)
#define deque_chunk_func(FUNC, TYPE) cctl_join(cctl_join(deque(TYPE), chunk), FUNC)
#define deque_func(FUNC, TYPE) cctl_join(deque(TYPE), FUNC)
#define deque_struct(TYPE) cctl_join(deque(TYPE), struct)

#define deque_init(TYPE, p_d) deque_func(init, TYPE)(p_d)
#define deque_resize(TYPE, p_d, size) deque_func(resize, TYPE)(p_d, size)
#define deque_reserve(TYPE, p_d, size) deque_func(reserve, TYPE)(p_d, size)
#define deque_clear(TYPE, p_d) deque_func(clear, TYPE)(p_d)
#define deque_free(TYPE, p_d) deque_func(free, TYPE)(p_d)
#define deque_push_back(TYPE, p_d, item) deque_func(push_back, TYPE)(p_d, item)
#define deque_pop_back(TYPE, p_d) deque_func(pop_back, TYPE)(p_d)
#define deque_at(TYPE, p_d, index) deque_func(at, TYPE)(p_d, index)
#define deque_front(TYPE, p_d) deque_func(front, TYPE)(p_d)
#define deque_back(TYPE, p_d) deque_func(back, TYPE)(p_d)

#define deque_fd(TYPE) \
	typedef struct deque_struct(TYPE) deque(TYPE);

#define deque_imp_h(TYPE) \
	chunk_imp_h(TYPE); \
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
	\
	bool deque_chunk_func(reserve, TYPE)(deque(TYPE)* p_d, size_t size); \
	bool deque_chunk_func(push_front, TYPE)(deque(TYPE)* p_d); \
	bool deque_chunk_func(push_back, TYPE)(deque(TYPE)* p_d); \
	bool deque_chunk_func(pop_front, TYPE)(deque(TYPE)* p_d); \
	bool deque_chunk_func(pop_back, TYPE)(deque(TYPE)* p_d); \
	chunk(TYPE)* deque_chunk_func(at, TYPE)(deque(TYPE)* p_d, size_t index); \
	chunk(TYPE)* deque_chunk_func(front, TYPE)(deque(TYPE)* p_d)); \
	chunk(TYPE)* deque_chunk_func(back, TYPE)(deque(TYPE)* p_d)); \
	\
	bool deque_func(resize, TYPE)(deque(TYPE)* p_d, size_t size); \
	void deque_func(init, TYPE)(deque(TYPE)* p_d); \
	void deque_func(clear, TYPE)(deque(TYPE)* p_d); \
	void deque_func(free, TYPE)(deque(TYPE)* p_d); \
	bool deque_func(push_front, TYPE)(deque(TYPE)* p_d, TYPE item); \
	bool deque_func(push_back, TYPE)(deque(TYPE)* p_d, TYPE item); \
	bool deque_func(pop_front, TYPE)(deque(TYPE)* p_d); \
	bool deque_func(pop_back, TYPE)(deque(TYPE)* p_d); \
	TYPE* deque_func(at, TYPE)(deque(TYPE)* p_d, size_t index); \
	TYPE* deque_func(front, TYPE)(deque(TYPE)* p_d); \
	TYPE* deque_func(back, TYPE)(deque(TYPE)* p_d);



#endif