#include "interpreter_data.h"

extern inline for_data for_data_init(void) {
	for_data data;
	data.variable_kwrd.u = 0;
	data.start.i = 0;
	data.end.i = 0;
	data.step.i = 1;
	data.foty = FOTY_I;
	data.is_infinite = false;
	return data;
}