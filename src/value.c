#include "value.h"

value value_init_i(int64_t x) {
	value v;
	v.i = x;
	return v;
}

value value_init_u(uint64_t x) {
	value v;
	v.u = x;
	return v;
}

value value_init_f(double x) {
	value v;
	v.f = x;
	return v;
}

value value_init_p(uint64_t* x) {
	value v;
	v.p = x;
	return v;
}