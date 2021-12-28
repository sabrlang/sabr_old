#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#if defined(_WIN32)
	#include <windows.h>
#else
	#include <libgen.h>
	#include <unistd.h>
	#if defined(__linux__)
		#include <linux/limits.h>
	#endif
#endif

#include "console.h"
#include "opcode.h"
#include "rbt.h"
#include "trie.h"
#include "value.h"

#include "compiler_cctl_define.h"
#include "control.h"
#include "operation.h"
#include "preproc.h"

typedef enum string_parse_mode_enum {
	STR_PARSE_NONE,
	STR_PARSE_SINGLE,
	STR_PARSE_DOUBLE,
	STR_PARSE_PREPROC
} string_parse_mode;

typedef enum comment_parse_mode_enum {
	CMNT_PARSE_NONE,
	CMNT_PARSE_LINE,
	CMNT_PARSE_STACK
} comment_parse_mode;

typedef struct compiler_struct {
	vector(cctl_ptr(char)) textcode_vector;
	vector(cctl_ptr(char)) filename_vector;
	vector(preproc_data) preproc_tokens_vector;
	vector(size_t) textcode_index_stack;
	vector(uint8_t) bytecode;
	vector(cctl_ptr(vector(control_data))) control_data_stack;
	trie dictionary;
	trie filename_trie;
	size_t dictionary_keyword_count;
	vector(size_t) line_count_stack;
	vector(size_t) column_count_stack;
	vector(size_t) column_count_prev_stack;
	mbstate_t convert_state;
} compiler;

bool compiler_init(compiler* comp);
bool compiler_del(compiler* comp);
bool compiler_compile(compiler* comp, char* input_filename, char* output_filename);
bool compiler_compile_source(compiler* comp, char* input_filename);
size_t compiler_load_code(compiler* comp, char* filename);
bool compiler_save_code(compiler* comp, char* filename);
bool compiler_push_code_data(compiler* comp, size_t line, size_t column, size_t column_prev, int index);
bool compiler_pop_code_data(compiler* comp);
bool compiler_tokenize(compiler* comp);
bool compiler_parse(compiler* comp, char* begin, char* end);
bool compiler_parse_word_token(compiler* comp, trie* trie_result);
bool compiler_parse_control_words(compiler* comp, trie* trie_result);
bool compiler_parse_keyword_value(compiler* comp, char* token);
bool compiler_parse_zero_begin_num(compiler* comp, char* token, size_t index, bool negate);
bool compiler_parse_base_n_num(compiler* comp, char* token, size_t index, bool negate, int base);
bool compiler_parse_num(compiler* comp, char* token);
bool compiler_parse_char(compiler* comp, char* token, bool push_length);
bool compiler_parsed_num_to_bytecode(compiler* comp, value v);
bool compiler_parse_struct_member(compiler* comp, char* token);
bool compiler_push_bytecode(compiler* comp, opcode op);
bool compiler_push_bytecode_with_value(compiler* comp, opcode op, value v);
bool compiler_push_bytecode_with_null(compiler* comp, opcode op);
bool compiler_push_preproc_token(compiler* comp, char* token);

inline size_t* compiler_current_column(compiler *comp) {
	return vector_back(size_t, &comp->column_count_stack);
}
inline size_t* compiler_current_column_prev(compiler* comp) {
	return vector_back(size_t, &comp->column_count_prev_stack);
}
inline size_t* compiler_current_line(compiler* comp) {
	return vector_back(size_t, &comp->line_count_stack);
}
inline size_t* compiler_current_file_index(compiler* comp) {
	return vector_back(size_t, &comp->textcode_index_stack);
}

bool is_can_be_keyword(char* token);

#endif