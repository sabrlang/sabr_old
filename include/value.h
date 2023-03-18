#ifndef __VALUE_H__
#define __VALUE_H__

#include <stdint.h>

#include "cctl.h"
typedef union value_union {
	int64_t i;
	uint64_t u;
	double f;
	uint64_t* p;
	uint8_t bytes[8];
} value;

value value_init_i(int64_t x);
value value_init_u(uint64_t x);
value value_init_f(double x);
value value_init_p(uint64_t* x);

#endif