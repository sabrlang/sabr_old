#include <stdio.h>

#include "interpreter.h"

interpreter inter;

int main(int argc, char* argv[]) {
	if (!interpreter_init(&inter)) return 1;

	if (argc == 1) {
		fputs("error : No input files\n", stderr);
		return 2;
	}
	if (!interpreter_memory_pool_init(&inter, 131072, 131072)) {
		fputs("error : Memory pool allocation failure", stderr);
		return 3;
	}
	interpreter_load_code(&inter, argv[1]);
	interpreter_run(&inter);

	interpreter_del(&inter);
	return 0;
}