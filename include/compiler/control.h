#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stddef.h>
#include <stdint.h>

typedef enum control_enum {
	CTRL_END,
	CTRL_IF,
	CTRL_ELSE,
	CTRL_LOOP,
	CTRL_WHILE,
	CTRL_BREAK,
	CTRL_CONTINUE,
	CTRL_FOR,
	CTRL_FROM,
	CTRL_TO,
	CTRL_STEP,
	CTRL_SWITCH,
	CTRL_CASE,
	CTRL_PASS,
	CTRL_FUNC,
	CTRL_MACRO,
	CTRL_DEFER,
	CTRL_RETURN,
	CTRL_STRUCT,
	CTRL_MEMBER,
	CTRL_IMPORT,
	CTRL_DEFINE,
	CTRL_EVAL,
	CTRL_CONCAT,
	CTRL_TDROP,
	CTRL_TNIP,
	CTRL_TDUP,
	CTRL_TOVER,
	CTRL_TTUCK,
	CTRL_TSWAP,
	CTRL_TROT,
	CTRL_2TDROP,
	CTRL_2TNIP,
	CTRL_2TDUP,
	CTRL_2TOVER,
	CTRL_2TTUCK,
	CTRL_2TSWAP,
	CTRL_2TROT
} control;

extern size_t control_len;
extern const char* control_names[];

typedef struct control_data_struct {
	size_t pos;
	size_t ctrl;
} control_data;

#endif