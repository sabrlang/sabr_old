#include "control.h"

const char* control_names[] = {
	"end",
	"if",
	"else",
	"loop",
	"while",
	"break",
	"continue",
	"switch",
	"case",
	"pass",
	"func",
	"macro",
	"return",
	"import"
};

size_t control_len = sizeof(control_names) / sizeof(char*);