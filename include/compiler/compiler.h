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

#include "opcode.h"
#include "rbt.h"
#include "trie.h"
#include "value.h"

#include "compiler_cctl_define.h"
#include "control.h"
#include "operation.h"

typedef enum string_parse_mode_enum {
	STR_PARSE_NONE,
	STR_PARSE_SINGLE,
	STR_PARSE_DOUBLE
} string_parse_mode;

typedef struct compiler_struct {
	vector(cctl_ptr(char)) textcode_vector;
	vector(cctl_ptr(char)) filename_vector;
	vector(uint8_t) bytecode;
	vector(cctl_ptr(vector(control_data))) control_data_stack;
	trie dictionary;
	size_t dictionary_keyword_count;
	size_t line_count;
	size_t column_count;
} compiler;

bool compiler_init(compiler* comp);
bool compiler_del(compiler* comp);
bool compiler_compile(compiler* comp, char* input_filename, char* output_filename);
size_t compiler_load_code(compiler* comp, char* filename);
bool compiler_save_code(compiler* comp, char* filename);
bool compiler_tokenize(compiler* comp, size_t index);
bool compiler_parse(compiler* comp, char* begin, char* end);
bool compiler_parse_word_token(compiler* comp, trie* trie_result);
bool compiler_parse_control_words(compiler* comp, trie* trie_result);
bool compiler_parse_keyword_value(compiler* comp, char* token);
bool compiler_parse_zero_begin_num(compiler* comp, char* token, size_t index, bool negate);
bool compiler_parse_base_n_num(compiler* comp, char* token, size_t index, bool negate, int base);
bool compiler_parse_num(compiler* comp, char* token);
bool compiler_parse_char(compiler* comp, char* token);
bool compiler_parsed_num_to_bytecode(compiler* comp, value v);
bool compiler_push_bytecode(compiler* comp, opcode op);
bool compiler_push_bytecode_with_value(compiler* comp, opcode op, value v);
bool compiler_push_bytecode_with_null(compiler* comp, opcode op);

#endif