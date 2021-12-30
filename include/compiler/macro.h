#ifndef __MACRO_H__
#define __MACRO_H__

typedef struct macro_data_struct {
	size_t filename_index;
	size_t code_index;
	size_t column;
	size_t line;
	bool is_concat;
} macro_data;

#endif