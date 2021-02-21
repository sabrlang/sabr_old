#include "compiler.h"

bool compiler_init(compiler* comp) {
	setlocale(LC_ALL, "en_US.utf8");

	vector_init(cctl_ptr(char), &comp->textcode_vector);
	vector_init(cctl_ptr(char), &comp->filename_vector);
	vector_init(uint8_t, &comp->bytecode);
	vector_init(cctl_ptr(vector(control_data)), &comp->control_data_stack);
	trie_init(&comp->dictionary);

	comp->dictionary_keyword_count = 0;
	comp->line_count = 1;
	comp->column_count = 0;

	trie* word;

	for (int i = 0; i < control_len; i++) {
		word = trie_insert(&comp->dictionary, control_names[i], WTT_CTRL);
		if (!word) goto FAILURE_DICT;
		word->data.u = i;
	}

	for (int i = 0; i < operation_len; i++) {
		word = trie_insert(&comp->dictionary, operation_names[i], WTT_OP);
		if (!word) goto FAILURE_DICT;
		word->data.u = operation_indices[i];
	}

	return true;

FAILURE_DICT:
	fputs("error : Dictionary memory allocation failure\n", stderr);
	return false;
}

bool compiler_del(compiler* comp) {
	for (int i = 0; i < comp->textcode_vector.size; i++) {
		free(*vector_at(cctl_ptr(char), &comp->filename_vector, i));
		free(*vector_at(cctl_ptr(char), &comp->textcode_vector, i));
	}

	vector_free(cctl_ptr(char), &comp->filename_vector);
	vector_free(cctl_ptr(char), &comp->textcode_vector);
	vector_free(uint8_t, &comp->bytecode);

	for (size_t i = 0; i < comp->control_data_stack.size; i++) {
		vector_free(control_data, *vector_at(cctl_ptr(vector(control_data)), &comp->control_data_stack, i));
	}
	vector_free(cctl_ptr(vector(control_data)), &comp->control_data_stack);
	trie_del(&comp->dictionary);

	return true;
}

bool compiler_compile(compiler* comp, char* input_filename, char* output_filename) {
	int index = compiler_load_code(comp, input_filename);
	if (!index) {
		fputs("error : Loading code failure\n", stderr);
		return false;
	}
	if (!compiler_tokenize(comp, index - 1)) {
		fputs("error : Tokenization failure\n", stderr);
		return false;
	}
	if (!compiler_save_code(comp, output_filename)) {
		fputs("error : File saving failure\n", stderr);
		return false;
	}
	return true;
}

size_t compiler_load_code(compiler* comp, char* filename) {
	FILE* file;
	size_t size;

	file = fopen(filename, "rb");
	if (!file) {
		fputs("error : File reading failure\n", stderr);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	int filename_size = strlen(filename) + 1;

	char* textcode = (char*) malloc(size + 2);
	char* filename_new = (char*) malloc(filename_size);
	if (!(textcode && filename_new)) {
		fclose(file);
		fputs("error : Textcode memory allocation failure\n", stderr);
		return 0;
	}

	int read_result = fread(textcode, size, 1, file);
	if (read_result != 1) {
		free(textcode);
		fclose(file);
		fputs("error : Entire reading failure\n", stderr);
		return 0;
	}

	textcode[size] = '\n';
	textcode[size + 1] = '\0';

#ifdef _WIN32
	memcpy_s(filename_new, filename_size, filename, filename_size);
#else
	memcpy(filename_new, filename, filename_size);
#endif

	if (!(
			vector_push_back(cctl_ptr(char), &comp->textcode_vector, textcode) &&
			vector_push_back(cctl_ptr(char), &comp->filename_vector, filename_new)
		)) {
		free(textcode);
		free(filename_new);
		fclose(file);
		fputs("error : Textcode vector memory allocation failure\n", stderr);
		return 0;
	}

	fclose(file);

	return comp->textcode_vector.size;
}

bool compiler_save_code(compiler* comp, char* filename) {
	FILE* file;
	file = fopen(filename, "wb");

	size_t result = fwrite(comp->bytecode.p_data, 1, comp->bytecode.size, file);

	fclose(file);

	if (result != comp->bytecode.size) {
		fputs("error : File writing failure\n", stderr);
		return false;
	}

	return true;
}

bool compiler_tokenize(compiler* comp, size_t index) {
	char* iterator = *vector_at(cctl_ptr(char), &comp->textcode_vector, index);
	char* begin;
	char* end;

	bool string_escape = false;
	string_parse_mode string_parse = STR_PARSE_NONE;
	bool space = true;
	bool comment = false;
	bool result;

	bool u8check = false;

	while (*iterator) {
		switch (*iterator) {
			case '\n': 
				comp->line_count++;
			case '\r':
				comp->column_count = 0;
				if (comment) {
					space = true;
					comment = false;
				}
			case '\t':
			case ' ': {
				if (comment) {

				}
				else {
					if (!space) {
						if (!string_parse) {
							end = iterator;
							result = compiler_parse(comp, begin, end);
							if (!result) {
								fputs("error : Parse failure\n", stderr);
								return false;
							}
							space = true;
						}
					}
				}
			} break;
			case '\'': {
				if (comment) {

				}
				else {
					if (string_parse) {
						if (string_escape) {
							string_escape = false;
						}
						else {
							if (string_parse == STR_PARSE_SINGLE) {
								string_parse = STR_PARSE_NONE;
							}
						}
					}
					else if (space) {
						space = false;
						begin = iterator;
						string_parse = STR_PARSE_SINGLE;
						string_escape = false;
					}
				}
			} break;
			case '\"': {
				if (comment) {

				}
				else {
					if (string_parse) {
						if (string_escape) {
							string_escape = false;
						}
						else {
							if (string_parse == STR_PARSE_DOUBLE) {
								string_parse = STR_PARSE_NONE;
							}
						}
					}
					else if (space) {
						space = false;
						begin = iterator;
						string_parse = STR_PARSE_DOUBLE;
						string_escape = false;
					}
				}
			} break;
			case '\\': {
				if (comment) {

				}
				else {
					if (string_parse) {
						if (!string_escape) {
							string_parse = true;
						}
					}
					if (space) {
						space = false;
						comment = true;
					}
				}
			} break;
			default: {
				if (comment) {
					
				}
				else {
					if (string_parse) {
						if (string_escape) {
							string_escape = false;
						}
					}
					if (space) {
						space = false;
						begin = iterator;
					}
				}
			}
		}
		iterator++;
		
		if (((signed char) *iterator) >= -64) {
			comp->column_count++;
		}
		
	}
	return true;
}

bool compiler_parse(compiler* comp, char* begin, char* end) {
	bool result;
	char temp = *end;
	*end = 0;

	trie* trie_result = trie_find(&comp->dictionary, begin);

	if (trie_result) {
		result = compiler_parse_word_token(comp, trie_result);
	}
	else {
		switch (*begin) {
			case '+': {
				result = compiler_parse_zero_begin_num(comp, begin, 1, false);
			} break;
			case '-': {
				result = compiler_parse_zero_begin_num(comp, begin, 1, true);
			} break;
			case '0': {
				result = compiler_parse_zero_begin_num(comp, begin, 0, false);
			} break;
			case '.':
			case '1' ... '9': {
				result = compiler_parse_num(comp, begin);
			} break;
			case '$': {
				result = compiler_parse_keyword_value(comp, begin + 1);
			} break;
			case '\'': {
				char temp_parse_char = *(end - 1);
				*(end - 1) = 0;
				result = compiler_parse_char(comp, begin + 1);
				*(end - 1) = temp_parse_char;
			} break;
			default: {
				result = false;
				fputs("error : Unknown keyword\n", stderr);
			}
		}
	}

	if (!result) {
		fprintf(stderr, "\'\'\'%s\'\'\' in line %zu, column %zu\n", begin, comp->line_count, comp->column_count);
	}

	*end = temp;
	return result;
}

bool compiler_parse_word_token(compiler* comp, trie* trie_result) {
	bool result = false;

	switch (trie_result->type) {
		case WTT_CTRL: {
			result = compiler_parse_control_words(comp, trie_result);
		} break;
		case WTT_KWRD: {
			value v = trie_result->data;
			if (!vector_push_back(uint8_t, &comp->bytecode, OP_CALL)) goto FAILURE;
			for (int i = 0; i < 8; i++) {
				if (!vector_push_back(uint8_t, &comp->bytecode, v.bytes[i])) goto FAILURE;
			}
			result = true;
		} break;
		case WTT_OP: {
			value v = trie_result->data;
			if (!vector_push_back(uint8_t, &comp->bytecode, v.u)) goto FAILURE;
			result = true;
		} break;
	}

	return result;

FAILURE:
	fputs("error : Bytecode memory allocation faliure\n", stderr);
	return false;
}

bool compiler_parse_control_words(compiler* comp, trie* trie_result) {
	control_data current_ctrl;
	current_ctrl.ctrl = trie_result->data.u;
	current_ctrl.pos = comp->bytecode.size;

	vector(control_data)* temp_ctrl_vec;

	switch (current_ctrl.ctrl) {
		case CTRL_IF: {
			temp_ctrl_vec = (vector(control_data)*) malloc(sizeof(vector(control_data)));
			if (!temp_ctrl_vec) goto FAILURE_CTRL_VECTOR;
			vector_init(control_data, temp_ctrl_vec);
			if (!vector_push_back(control_data, temp_ctrl_vec, current_ctrl)) goto FAILURE_CTRL_VECTOR;
			if (!vector_push_back(cctl_ptr(vector(control_data)), &comp->control_data_stack, temp_ctrl_vec)) goto FAILURE_CTRL_STACK;
			if (!vector_push_back(uint8_t, &comp->bytecode, OP_IF)) goto FAILURE_BYTECODE;
			for (int i = 0; i < 8; i++) {
				if (!vector_push_back(uint8_t, &comp->bytecode, 0)) goto FAILURE_BYTECODE;
			}
		} break;
		case CTRL_ELSE: {
			if (!comp->control_data_stack.size) goto FAILURE_CTRL;
			temp_ctrl_vec = *vector_back(cctl_ptr(vector(control_data)), &comp->control_data_stack);
			if (!vector_push_back(control_data, temp_ctrl_vec, current_ctrl)) goto FAILURE_CTRL_VECTOR;
			if (!vector_push_back(uint8_t, &comp->bytecode, OP_JUMP)) goto FAILURE_BYTECODE;
			for (int i = 0; i < 8; i++) {
				if (!vector_push_back(uint8_t, &comp->bytecode, 0)) goto FAILURE_BYTECODE;
			}
		} break;
		case CTRL_LOOP: {
			temp_ctrl_vec = (vector(control_data)*) malloc(sizeof(vector(control_data)));
			if (!temp_ctrl_vec) goto FAILURE_CTRL_VECTOR;
			vector_init(control_data, temp_ctrl_vec);
			if (!vector_push_back(control_data, temp_ctrl_vec, current_ctrl)) goto FAILURE_CTRL_VECTOR;
			if (!vector_push_back(cctl_ptr(vector(control_data)), &comp->control_data_stack, temp_ctrl_vec)) goto FAILURE_CTRL_STACK;
		} break;
		case CTRL_WHILE: {
			if (!comp->control_data_stack.size) goto FAILURE_CTRL;
			temp_ctrl_vec = *vector_back(cctl_ptr(vector(control_data)), &comp->control_data_stack);
			if (!vector_push_back(control_data, temp_ctrl_vec, current_ctrl)) goto FAILURE_CTRL_VECTOR;
			if (!vector_push_back(uint8_t, &comp->bytecode, OP_IF)) goto FAILURE_BYTECODE;
			for (int i = 0; i < 8; i++) {
				if (!vector_push_back(uint8_t, &comp->bytecode, 0)) goto FAILURE_BYTECODE;
			}
		} break;
		case CTRL_CONTINUE:
		case CTRL_BREAK: {
			if (!comp->control_data_stack.size) goto FAILURE_CTRL;
			temp_ctrl_vec = *vector_back(cctl_ptr(vector(control_data)), &comp->control_data_stack);
			if (!vector_push_back(control_data, temp_ctrl_vec, current_ctrl)) goto FAILURE_CTRL_VECTOR;
			if (!vector_push_back(uint8_t, &comp->bytecode, OP_JUMP)) goto FAILURE_BYTECODE;
			for (int i = 0; i < 8; i++) {
				if (!vector_push_back(uint8_t, &comp->bytecode, 0)) goto FAILURE_BYTECODE;
			}
		} break;
		case CTRL_FUNC: {
			temp_ctrl_vec = (vector(control_data)*) malloc(sizeof(vector(control_data)));
			if (!temp_ctrl_vec) goto FAILURE_CTRL_VECTOR;
			vector_init(control_data, temp_ctrl_vec);
			if (!vector_push_back(control_data, temp_ctrl_vec, current_ctrl)) goto FAILURE_CTRL_VECTOR;
			if (!vector_push_back(cctl_ptr(vector(control_data)), &comp->control_data_stack, temp_ctrl_vec)) goto FAILURE_CTRL_STACK;
			if (!vector_push_back(uint8_t, &comp->bytecode, OP_FUNC)) goto FAILURE_BYTECODE;
			for (int i = 0; i < 8; i++) {
				if (!vector_push_back(uint8_t, &comp->bytecode, 0)) goto FAILURE_BYTECODE;
			}
		} break;
		case CTRL_END: {
			value pos;
			if (comp->control_data_stack.size == 0) goto FAILURE_CTRL_STACK;
			temp_ctrl_vec = *vector_back(cctl_ptr(vector(control_data)), &comp->control_data_stack);
			control_data* first_ctrl = vector_front(control_data, temp_ctrl_vec);
			control_data* while_else_ctrl = NULL;
			switch (first_ctrl->ctrl) {
				case CTRL_IF: {
					for (
						control_data* iter = vector_at(control_data, temp_ctrl_vec, 1);
						iter <= vector_back(control_data, temp_ctrl_vec);
						iter++
					) {
						switch (iter->ctrl) {
							case CTRL_ELSE: {
								while_else_ctrl = iter;
							} break;
							case CTRL_BREAK:
							case CTRL_CONTINUE: {
								vector(control_data)* next_ctrl_vec;
								if (comp->control_data_stack.size < 2) goto FAILURE_CTRL_STACK;
								next_ctrl_vec = *vector_at(cctl_ptr(vector(control_data)), &comp->control_data_stack, comp->control_data_stack.size - 2);
								if (!vector_push_back(control_data, next_ctrl_vec, *iter)) goto FAILURE_CTRL_VECTOR;
							} break;
							default: {
								goto FAILURE_CTRL;
							}
						}
					}
					if (while_else_ctrl) {
						pos.u = while_else_ctrl->pos + 9;
						for (int i = 0; i < 8; i++) {
							*vector_at(uint8_t, &comp->bytecode, first_ctrl->pos + 1 + i) = pos.bytes[i];
						}
						pos.u = current_ctrl.pos;
						for (int i = 0; i < 8; i++) {
							*vector_at(uint8_t, &comp->bytecode, while_else_ctrl->pos + 1 + i) = pos.bytes[i];
						}
					}
					else {
						pos.u = current_ctrl.pos;
						for (int i = 0; i < 8; i++) {
							*vector_at(uint8_t, &comp->bytecode, first_ctrl->pos + 1 + i) = pos.bytes[i];
						}
					}
				} break;
				case CTRL_LOOP: {
					for (
						control_data* iter = vector_at(control_data, temp_ctrl_vec, 1);
						iter <= vector_back(control_data, temp_ctrl_vec);
						iter++
					) {
						switch (iter->ctrl) {
							case CTRL_WHILE: {
								while_else_ctrl = iter;
								pos.u = current_ctrl.pos + 9;
								for (int i = 0; i < 8; i++) {
									*vector_at(uint8_t, &comp->bytecode, while_else_ctrl->pos + 1 + i) = pos.bytes[i];
								}
							} break;
							case CTRL_BREAK: {
								pos.u = current_ctrl.pos + 9;
								for (int i = 0; i < 8; i++) {
									*vector_at(uint8_t, &comp->bytecode, iter->pos + 1 + i) = pos.bytes[i];
								}
							} break;
							case CTRL_CONTINUE: {
								pos.u = first_ctrl->pos;
								for (int i = 0; i < 8; i++) {
									*vector_at(uint8_t, &comp->bytecode, iter->pos + 1 + i) = pos.bytes[i];
								}
							} break;
							default: {
								goto FAILURE_CTRL;
							}
						}
					}
					pos.u = first_ctrl->pos;
					if (!vector_push_back(uint8_t, &comp->bytecode, OP_JUMP)) goto FAILURE_BYTECODE;
					for (int i = 0; i < 8; i++) {
						if (!vector_push_back(uint8_t, &comp->bytecode, pos.bytes[i])) goto FAILURE_BYTECODE;
					}
				} break;
				case CTRL_FUNC: {
					if (temp_ctrl_vec->size > 1) goto FAILURE_CTRL;
					pos.u = current_ctrl.pos + 1;
					for (int i = 0; i < 8; i++) {
						*vector_at(uint8_t, &comp->bytecode, first_ctrl->pos + 1 + i) = pos.bytes[i];
					}
					if (!vector_push_back(uint8_t, &comp->bytecode, OP_RETURN)) goto FAILURE_BYTECODE;
				} break;
			}
			vector_free(control_data, temp_ctrl_vec);
			if (!vector_pop_back(cctl_ptr(vector(control_data)), &comp->control_data_stack)) goto FAILURE_CTRL_STACK;
		} break;
	}

	return true;
FAILURE_CTRL:
	fputs("error : Invalid control syntax\n", stderr);
	return false;
FAILURE_CTRL_VECTOR:
	fputs("error : Control data vector memory allocation faliure\n", stderr);
	return false;
FAILURE_CTRL_STACK:
	fputs("error : Control data stack memory allocation faliure\n", stderr);
	return false;
FAILURE_BYTECODE:
	fputs("error : Bytecode memory allocation faliure\n", stderr);
	return false;
}

bool compiler_parse_keyword_value(compiler* comp, char* token) {
	trie* word;
	
	word = trie_find(&comp->dictionary, token);

	if (word) {
		if (word->type != WTT_KWRD) {
			fputs("error : Control words and built-in-functions cannot be keywords\n", stderr);
			return false;
		}
	}
	else {
		char* iter = token;
		switch (*iter) {
			case '+':
			case '-':
			case '.': {
				iter++;
				switch (*iter) {
					case '0' ... '9': {
						goto FAILURE_INVALID_KEYWORD;
					} break;
				}
			} break;
			case '0' ... '9':
			case '@':
			case '#':
			case '$':
			case '\'':
			case '\"':
			case '\0': {
				goto FAILURE_INVALID_KEYWORD;
			} break;
		}

		comp->dictionary_keyword_count++;
		word = trie_insert(&comp->dictionary, token, WTT_KWRD);
		if (!word) {
			fputs("error : Dictionary memory allocation failure\n", stderr);
			return false;
		}
		word->data.u = comp->dictionary_keyword_count;
	}

	if (!vector_push_back(uint8_t, &comp->bytecode, OP_VALUE)) goto FAILURE_BYTECODE;
	for (int i = 0; i < 8; i++) {
		if (!vector_push_back(uint8_t, &comp->bytecode, word->data.bytes[i])) goto FAILURE_BYTECODE;
	}

	return true;
FAILURE_BYTECODE:
	fputs("error : Bytecode memory allocation faliure\n", stderr);
	return false;
FAILURE_INVALID_KEYWORD:
	fputs("error : Invalid keyword\n", stderr);
	return false;
}

bool compiler_parse_zero_begin_num(compiler* comp, char* token, size_t index, bool negate) {
	bool result;

	if (token[index] == '0') {
		switch (token[index + 1]) {
			case 'x': {
				result = compiler_parse_base_n_num(comp, token, index, negate, 16);
			} break;
			case 'o': {
				result = compiler_parse_base_n_num(comp, token, index, negate, 8);
			} break;
			case 'b': {
				result = compiler_parse_base_n_num(comp, token, index, negate, 2);
			} break;
			default: {
				result = compiler_parse_num(comp, token);
			}
		}
	}
	else result = compiler_parse_num(comp, token);

	return result;
}

bool compiler_parse_base_n_num(compiler* comp, char* token, size_t index, bool negate, int base) {
	char* temp = token + index + 2;
	char* stop;
	bool result;
	value v;

	errno = 0;
	v.i = strtoll(temp, &stop, base);

	if (errno == ERANGE) {
		if (v.i == LLONG_MAX) {
			v.u = strtoull(temp, &stop, base);
		}
	}
	if (negate) v.i = -v.i;
	
	if (*stop) {
		fputs("error : Number parsing failure\n", stderr);
		return false;
	}

	result = compiler_parsed_num_to_bytecode(comp, v);

	return result;
}

bool compiler_parse_num(compiler* comp, char* token) {
	char* stop;
	bool result;
	value v;

	errno = 0;
	if (strchr(token, '.')) {
		v.f = strtod(token, &stop);
	}
	else {
		v.i = strtoll(token, &stop, 10);
		if (errno == ERANGE) {
			if (v.i == LLONG_MAX) {
				v.u = strtoull(token, &stop, 10);
			}
		}
	}
	if (*stop) {
		fputs("error : Number parsing failure\n", stderr);
		return false;
	}
	result = compiler_parsed_num_to_bytecode(comp, v);

	return result;
}

bool compiler_parse_char(compiler* comp, char* token) {
	value v;

	int num_parse_count;
	int num_parse_max;

	vector(value) value_reverser;
	vector_init(value, &value_reverser);

	while (*token) {
		if (((signed char)*token) > -1) {
			char* num_pasre_stop = NULL;
			char num_parse[9] = {0, };
			num_parse_count = 0;
			if (*token == '\\') {
				token++;
				switch (*token) {
					case 'a': v.u = '\a'; break;
					case 'b': v.u = '\b'; break;
					case 'e': v.u = '\e'; break;
					case 'f': v.u = '\f'; break;
					case 'n': v.u = '\n'; break;
					case 'r': v.u = '\r'; break;
					case 't': v.u = '\t'; break;
					case 'v': v.u = '\v'; break;
					case '\\': v.u = '\\'; break;
					case '\'': v.u = '\''; break;
					case '\"': v.u = '\"'; break;
						break;
					case '0' ... '7': {
						while ((*token >= '0') && (*token <= '7') && (num_parse_count < 3)) {
							num_parse[num_parse_count] = *token;
							token++;
							num_parse_count++;
						}
						if (num_parse_count != 3) {
							goto FAILURE_ESCAPE;
						}
						v.u = strtoull(num_parse, &num_pasre_stop, 8);
					} break;
					case 'x': {
						token++;
						while (
							(((*token >= '0') && (*token <= '7')) || ((*token >= 'a') && (*token <= 'f')) || ((*token >= 'A') && (*token <= 'F'))) && (num_parse_count < 2))
						{
							num_parse[num_parse_count] = *token;
							token++;
							num_parse_count++;
						}
						if (num_parse_count != 2) {
							goto FAILURE_ESCAPE;
						}
						v.u = strtoull(num_parse, &num_pasre_stop, 16);
					} break;
					case 'u': {
						token++;
						while (
							(((*token >= '0') && (*token <= '7')) || ((*token >= 'a') && (*token <= 'f')) || ((*token >= 'A') && (*token <= 'F'))) && (num_parse_count < 4))
						{
							num_parse[num_parse_count] = *token;
							token++;
							num_parse_count++;
						}
						if (num_parse_count != 4) {
							goto FAILURE_ESCAPE;
						}
						v.u = strtoull(num_parse, &num_pasre_stop, 16);
					} break;
					case 'U': {
						token++;
						while (
							(((*token >= '0') && (*token <= '7')) || ((*token >= 'a') && (*token <= 'f')) || ((*token >= 'A') && (*token <= 'F'))) && (num_parse_count < 8))
						{
							num_parse[num_parse_count] = *token;
							token++;
							num_parse_count++;
						}
						if (num_parse_count != 8) {
							goto FAILURE_ESCAPE;
						}
						v.u = strtoull(num_parse, &num_pasre_stop, 16);
					} break;
					default: {
						goto FAILURE_ESCAPE;
					}
				}
			}
			else if ((*token == '\'') || (*token == '\"')) {
				vector_free(value, &value_reverser);
				fputs("error : String parse failure\n", stderr);
				return false;
			}
			else {
				v.u = *token;
			}
			token++;
		}
		else {
			char32_t out;
			size_t rc;
			mbstate_t state;
			rc = mbrtoc32(&out, token, 100, &state);
			if ((rc > -4) || (rc == 0)) goto FAILURE_UNICODE;
			token += rc;
			v.u = out;
		}
		if (!vector_push_back(value, &value_reverser, v)) goto FAILURE_VECTOR;
	}
	
	for (size_t i = value_reverser.size - 1; i < -1; i--) {
		if (!compiler_parsed_num_to_bytecode(comp, *vector_at(value, &value_reverser, i))) return false;
	}

	vector_free(value, &value_reverser);
	return true;

FAILURE_ESCAPE:
	vector_free(value, &value_reverser);
	fputs("error : Escape sequence parsing faliure\n", stderr);
	return false;

FAILURE_UNICODE:
	vector_free(value, &value_reverser);
	fputs("error : Unicode decoding faliure\n", stderr);
	return false;

FAILURE_VECTOR:
	vector_free(value, &value_reverser);
	fputs("error : Unicode inserter vector memory allocation failure\n", stderr);
	return false;
}

bool compiler_parsed_num_to_bytecode(compiler* comp, value v) {

	if (!vector_push_back(uint8_t, &comp->bytecode, OP_VALUE)) goto FAILURE;

	for (int i = 0; i < 8; i++) {
		if (!vector_push_back(uint8_t, &comp->bytecode, v.bytes[i])) goto FAILURE;
	}
	return true;

FAILURE:
	fputs("error : Bytecode memory allocation faliure\n", stderr);
	return false;
}