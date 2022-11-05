#include <stdio.h>
#include <stdlib.h>

#include "compregex.h"
#include "util/misc.h"

int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "%s <expression régulière...>\n", argv[0]);
		exit(1);
	}
	
	int retcode = system("rm -f out/png/param*.png");
	if(retcode != 0) {
		fprintf(stderr, "avertissement: impossible de supprimer les anciens dessins (%i)\n", retcode);
	}
	
	for(int i = 1; i < argc; ++i) {
		char filename[16]; // len("param") + ceil(log10(2^31)) + len('\0')
		snprintf(&filename[0], 16, "param%i", i - 1);
		
		AFN A = compile(argv[i]);
		afn_dot(A, &filename[0]);
		afn_free(A);
	}
	
	printf("Toutes les expressions régulières ont été dessinées.\n");
}

