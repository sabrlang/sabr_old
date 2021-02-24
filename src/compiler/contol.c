#include "control.h"

const char* control_names[] = {
	"end",
	"if",
	"else",
	"loop",
	"while",
	"break",
	"continue",
	"func",
	"macro"
};

size_t control_len = sizeof(control_names) / sizeof(char*);