#include <stdio.h>
#include <stdlib.h>

#include "compregex.h"

int main(int argc, char *argv[]) {
	if(argc != 3) {
		fprintf(stderr, "%s <expression régulière> <chaîne à tester>\n", argv[0]);
		exit(1);
	}
	
	AFN A = compile(argv[1]);
	afn_dot(A, "grep");
	
	if(afn_simuler(A, argv[2])) {
		printf("\"%s\" est acceptée\n", argv[2]);
	}
	else {
		printf("\"%s\" est rejetée\n", argv[2]);
	}
	
	afn_free(A);
}
