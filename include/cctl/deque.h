#ifndef __CCTL_DEQUE_H__
#define __CCTL_DEQUE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cctl.h"

#define cctl_deque_chunk_max 512

#define chunk(TYPE) cctl_join(TYPE, chunk)
#define chunk_func(FUNC, TYPE) cctl_join(chunk(TYPE), FUNC)
#define chunk_struct(TYPE) cctl_join(chunk(TYPE), struct)

#define chunk_front(T, p_c) ((p_c)->p_data + (p_c)->begin)
#define chunk_back(T, p_c) ((p_c)->p_data + (p_c)->end)

#define chunk_imp_h(TYPE) \
	typedef struct chunk_struct(TYPE) chunk(TYPE); \
	\
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
	TYPE* chunk_func(at, TYPE)(chunk(TYPE)* p_c, size_t index);

#define chunk_imp_c(TYPE) \
	bool chunk_func(reserve, TYPE)(chunk(TYPE)* p_c, size_t size) { \
		if (p_c->capacity == size) return true; \
		size_t capacity = 1; \
		if (size > 0) while (capacity < size) capacity <<= 1; \
		if (p_c->capacity < size) { \
			TYPE* p_temp = NULL; \
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
	void chunk_func(init, TYPE)(chunk(TYPE)* p_c) { \
		p_c->p_data = NULL; \
		p_c->begin = 0; \
		p_c->end = -1; \
		p_c->size = 0; \
		p_c->capacity = 0; \
		p_c->allocated = false; \
		return; \
	} \
	\
	void chunk_func(clear, TYPE)(chunk(TYPE)* p_c) { \
		memset(p_c->p_data, 0, p_c->capacity * sizeof(TYPE)); \
		p_c->begin = 0; \
		p_c->end = -1; \
		p_c->size = 0; \
		return; \
	} \
	\
	void chunk_func(free, TYPE)(chunk(TYPE)* p_c) { \
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

#define deque_chunk_front(TYPE, p_d) ((p_d)->p_data + (p_d)->chunk_begin)
#define deque_chunk_back(TYPE, p_d) ((p_d)->p_data + (p_d)->chunk_end)
#define deque_front(TYPE, p_d) chunk_front(TYPE, deque_chunk_front(TYPE, p_d))
#define deque_back(TYPE, p_d) chunk_back(TYPE, deque_chunk_back(TYPE, p_d))

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
	\
	bool deque_func(resize, TYPE)(deque(TYPE)* p_d, size_t size); \
	void deque_func(init, TYPE)(deque(TYPE)* p_d); \
	void deque_func(clear, TYPE)(deque(TYPE)* p_d); \
	void deque_func(free, TYPE)(deque(TYPE)* p_d); \
	bool deque_func(push_front, TYPE)(deque(TYPE)* p_d, TYPE item); \
	bool deque_func(push_back, TYPE)(deque(TYPE)* p_d, TYPE item); \
	bool deque_func(pop_front, TYPE)(deque(TYPE)* p_d); \
	bool deque_func(pop_back, TYPE)(deque(TYPE)* p_d); \
	TYPE* deque_func(at, TYPE)(deque(TYPE)* p_d, size_t index);

#define deque_imp_c(TYPE) \
	chunk_imp_c(TYPE); \
	\
	bool deque_chunk_func(reserve, TYPE)(deque(TYPE)* p_d, size_t size) { \
		if (p_d->chunk_capacity == size) return true; \
		size_t capacity = 1; \
		if (size > 0) while (capacity < size) capacity <<= 1; \
		if (p_d->chunk_capacity < size) { \
			chunk(TYPE)* p_temp = NULL; \
			if (!(p_temp = (chunk(TYPE)*) calloc(capacity, sizeof(chunk(TYPE))))) return false; \
			if (p_d->allocated) { \
				if (p_d->chunk_begin > p_d->chunk_end) { \
					size_t temp_size = p_d->chunk_capacity - p_d->chunk_begin; \
					memcpy(p_temp, p_d->p_data + p_d->chunk_begin, temp_size * sizeof(chunk(TYPE))); \
					memcpy(p_temp + temp_size, p_d->p_data, (p_d->chunk_end + 1) * sizeof(chunk(TYPE))); \
				} \
				else memcpy(p_temp, p_d->p_data + p_d->chunk_begin, p_d->chunk_capacity * sizeof(chunk(TYPE))); \
				free(p_d->p_data); \
			} \
			else p_d->allocated = true; \
			p_d->p_data = p_temp; \
			p_d->chunk_begin = 0; \
			p_d->chunk_end = p_d->chunk_count - 1; \
			p_d->chunk_capacity = capacity; \
		} \
		return true; \
	} \
	\
	bool deque_chunk_func(push_front, TYPE)(deque(TYPE)* p_d) { \
		chunk(TYPE) temp; \
		chunk_func(init, TYPE)(&temp); \
		if (deque_chunk_func(reserve, TYPE)(p_d, p_d->chunk_count + 1)) { \
			p_d->chunk_count++; \
			if (p_d->chunk_begin == 0) p_d->chunk_begin = p_d->chunk_capacity; \
			p_d->chunk_begin--; \
			*(p_d->p_data + p_d->chunk_begin) = temp; \
			return true; \
		} \
		return false; \
	} \
	\
	bool deque_chunk_func(push_back, TYPE)(deque(TYPE)* p_d) { \
		chunk(TYPE) temp; \
		chunk_func(init, TYPE)(&temp); \
		if (deque_chunk_func(reserve, TYPE)(p_d, p_d->chunk_count + 1)) { \
			p_d->chunk_count++; \
			if (p_d->chunk_end >= p_d->chunk_capacity - 1) p_d->chunk_end = -1; \
			p_d->chunk_end++; \
			*(p_d->p_data + p_d->chunk_end) = temp; \
			return true; \
		} \
		return false; \
	} \
	\
	bool deque_chunk_func(pop_front, TYPE)(deque(TYPE)* p_d) { \
		if (p_d->chunk_count == 0) return false; \
		p_d->chunk_count--; \
		chunk_func(free, TYPE)(deque_chunk_front(TYPE, p_d)); \
		if (p_d->chunk_begin == p_d->chunk_capacity - 1) p_d->chunk_begin = -1; \
		p_d->chunk_begin++; \
		return true; \
	} \
	\
	bool deque_chunk_func(pop_back, TYPE)(deque(TYPE)* p_d) { \
		if (p_d->chunk_count == 0) return false; \
		p_d->chunk_count--; \
		chunk_func(free, TYPE)(deque_chunk_back(TYPE, p_d)); \
		if (p_d->chunk_end == 0) p_d->chunk_end = p_d->chunk_capacity; \
		p_d->chunk_end--; \
		return true; \
	} \
	\
	chunk(TYPE)* deque_chunk_func(at, TYPE)(deque(TYPE)* p_d, size_t index) { \
		if (p_d->chunk_begin > p_d->chunk_end) { \
			size_t temp = p_d->chunk_capacity - 1 - p_d->chunk_begin; \
			if (index > temp) \
				return p_d->p_data + index - temp - 1; \
		} \
		return p_d->p_data + p_d->chunk_begin + index; \
	} \
	\
	bool deque_func(resize, TYPE)(deque(TYPE)* p_d, size_t new_size) { \
		if (new_size > p_d->size) { \
			TYPE none; \
			memset(&none, 0, sizeof(TYPE)); \
			for (size_t i = p_d->size; i < new_size; i++) { \
				if (!deque_func(push_back, TYPE)(p_d, none)) return false; \
			} \
		} \
		else if (new_size < p_d->size) { \
			size_t temp = p_d->size; \
			for (size_t i = new_size; i < temp; i++) { \
				if (!(deque_func(pop_back, TYPE)(p_d))) return false; \
			} \
		} \
		return true; \
	} \
	\
	void deque_func(init, TYPE)(deque(TYPE)* p_d) { \
		p_d->p_data = NULL; \
		p_d->chunk_begin = 0; \
		p_d->chunk_end = 0; \
		p_d->chunk_count = 0; \
		p_d->chunk_capacity = 0; \
		p_d->allocated = false; \
		p_d->size = 0; \
	} \
	\
	void deque_func(clear, TYPE)(deque(TYPE)* p_d) { \
		for (size_t i = 0; i < p_d->chunk_count; i++) { \
			chunk_func(free, TYPE)(deque_chunk_func(at, TYPE)(p_d, i)); \
		} \
		p_d->chunk_count = 0; \
		p_d->chunk_begin = 0; \
		p_d->chunk_end = 0; \
		p_d->size = 0; \
	} \
	\
	void deque_func(free, TYPE)(deque(TYPE)* p_d) { \
		for (size_t i = 0; i < p_d->chunk_count; i++) { \
			chunk_func(free, TYPE)(deque_chunk_func(at, TYPE)(p_d, i)); \
		} \
		free(p_d->p_data); \
		deque_func(init, TYPE)(p_d); \
	} \
	\
	bool deque_func(push_front, TYPE)(deque(TYPE)* p_d, TYPE item) { \
		if (p_d->chunk_count > 0) { \
			if (deque_chunk_front(TYPE, p_d)->size >= cctl_deque_chunk_max) { \
				if (!deque_chunk_func(push_front, TYPE)(p_d)) return false; \
			} \
		} \
		else { \
			if (!deque_chunk_func(push_front, TYPE)(p_d)) return false; \
		} \
		if (!chunk_func(push_front, TYPE)(deque_chunk_front(TYPE, p_d), item)) return false; \
		p_d->size++; \
		return true; \
	} \
	\
	bool deque_func(push_back, TYPE)(deque(TYPE)* p_d, TYPE item) { \
		if (p_d->chunk_count > 0) { \
			if (deque_chunk_back(TYPE, p_d)->size >= cctl_deque_chunk_max) { \
				if (!deque_chunk_func(push_back, TYPE)(p_d)) return false; \
			} \
		} \
		else { \
			if (!deque_chunk_func(push_back, TYPE)(p_d)) return false; \
		} \
		if (!chunk_func(push_back, TYPE)(deque_chunk_back(TYPE, p_d), item)) return false; \
		p_d->size++; \
		return true; \
	} \
	\
	bool deque_func(pop_front, TYPE)(deque(TYPE)* p_d) { \
		if (p_d->chunk_count == 0) return false; \
		if (deque_chunk_front(TYPE, p_d)->size == 0) { \
			if (!deque_chunk_func(pop_front, TYPE)(p_d)) return false; \
		} \
		if (!chunk_func(pop_front, TYPE)(deque_chunk_front(TYPE, p_d))) return false; \
		if (deque_chunk_front(TYPE, p_d)->size == 0) { \
			if (!deque_chunk_func(pop_front, TYPE)(p_d)) return false; \
		} \
		p_d->size--; \
		return true; \
	} \
	\
	bool deque_func(pop_back, TYPE)(deque(TYPE)* p_d) { \
		if (p_d->chunk_count == 0) return false; \
		if (deque_chunk_back(TYPE, p_d)->size == 0) { \
			if (!deque_chunk_func(pop_back, TYPE)(p_d)) return false; \
		} \
		if (!chunk_func(pop_back, TYPE)(deque_chunk_back(TYPE, p_d))) return false; \
		if (deque_chunk_back(TYPE, p_d)->size == 0) { \
			if (!deque_chunk_func(pop_back, TYPE)(p_d)) return false; \
		} \
		p_d->size--; \
		return true; \
	} \
	\
	TYPE* deque_func(at, TYPE)(deque(TYPE)* p_d, size_t index) { \
		size_t front_size = deque_chunk_front(TYPE, p_d)->size; \
		size_t chunk_index; \
		if (index < front_size) \
			return chunk_func(at, TYPE)(deque_chunk_front(TYPE, p_d), index); \
		index -= front_size; \
		chunk_index = index / cctl_deque_chunk_max + 1; \
		index %= cctl_deque_chunk_max; \
		return chunk_func(at, TYPE)(deque_chunk_func(at, TYPE)(p_d, chunk_index), index); \
	}

#endif