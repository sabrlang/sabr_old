#ifndef __WIN_H__
#define __WIN_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
	ssize_t getdelim(char** buffer, size_t* buffer_size, int delimiter, FILE* file);
	ssize_t getline(char **buffer, size_t* buffer_size, FILE* file);
#endif

#endif