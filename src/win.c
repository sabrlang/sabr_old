#include "win.h"

#ifdef _WIN32
	ssize_t getdelim(char** buffer, size_t* buffer_size, int delimiter, FILE* file) {
		int n;
		while(true) {
			n = fgetc(file);
			printf("%u ", n);
			if (n == delimiter) break;
		}
	}
	ssize_t getline(char **buffer, size_t* buffer_size, FILE* file) {
		return getdelim(buffer, buffer_size, '\n', file);
	}
#endif