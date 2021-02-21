#include <stdio.h>

#include "compiler.h"

int main(int argc, char* argv[]) {
	
	compiler comp;
	char* output_filename;
	if (!compiler_init(&comp)) return 1;

	if (argc == 1) {
		fputs("error : No input files\n", stderr);
		return 2;
	}
	if (argc == 2) output_filename = "out.sabre";
	else output_filename = argv[2];
	if (!compiler_compile(&comp, argv[1], output_filename)) {
		fputs("error : Compilation failure\n", stderr);
		return 3;
	}
	
	compiler_del(&comp);

	return 0;
}