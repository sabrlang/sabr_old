#ifndef __PREPROC_H__
#define __PREPROC_H__

typedef struct preproc_data_struct {
	char* code;
	size_t filename_index;
	size_t code_index;
	size_t column;
	size_t line;
	bool is_concated;
} preproc_data;

#endif