#include <stdio.h>
#include <stdlib.h>

#include "compregex.h"

int main(int argc, char *argv[]) {
	if(argc != 3) {
		fprintf(stderr, "%s <expression régulière> <chaîne>\n", argv[0]);
		exit(1);
	}
	
	AFN A = compile(argv[1]);
}
