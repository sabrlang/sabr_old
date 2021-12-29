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
	CTRL_CONCAT
} control;

extern size_t control_len;
extern const char* control_names[];

typedef struct control_data_struct {
	size_t pos;
	size_t ctrl;
} control_data;

#endif