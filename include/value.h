#ifndef __VALUE_H__
#define __VALUE_H__

#include <stdint.h>

typedef union value_union {
	int64_t i;
	uint64_t u;
	double f;
	uint64_t* p;
	uint8_t bytes[8];
} value;

#endif