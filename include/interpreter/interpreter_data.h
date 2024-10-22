#ifndef __INTERPRETER_DATA_H__
#define __INTERPRETER_DATA_H__

#include <stdbool.h>
#include <stddef.h>

#include "value.h"

typedef enum for_type_enum {
	FOTY_I,
	FOTY_U,
	FOTY_F
} for_type;

typedef struct for_data_struct for_data;
struct for_data_struct {
	value variable_kwrd;
	value* variable_addr;
	value start;
	value end;
	value step;
	for_type foty;
	bool is_infinite;
};

typedef struct cs_data_struct cs_data;
struct cs_data_struct {
	size_t pos;
	size_t switch_stack_size;
	size_t for_data_stack_size;
};

for_data for_data_init(void);

#endif