#ifndef __PREPROC_H__
#define __PREPROC_H__

typedef struct preproc_data_struct {
	char* code;
	size_t column;
	size_t line;
} preproc_data;

#endif