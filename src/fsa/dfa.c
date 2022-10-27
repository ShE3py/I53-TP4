#include "fsa/dfa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fsa/mod.h"
#include "misc.h"

Dfa dfa_construct(int Q, int q0, int **F, int lenF, char **Sigma, int lenSigma) {
	check_param("Q", Q >= 0);
	check_param("q0", q0 >= 0 && q0 <= Q);
	check_param("F", F != NULL && *F != NULL);
	check_param("lenF", lenF > 0);
	check_param("Sigma", Sigma != NULL && *Sigma != NULL);
	check_param("lenSigma", lenSigma > 0);
	
	Dfa A = checked_malloc(sizeof(struct Dfa));
	A->Q = Q;
	A->q0 = q0;
	A->F = *F;
	A->lenF = lenF;
	A->Sigma = *Sigma;
	A->lenSigma = lenSigma;
	
	for(int i = 0; i < SYMBOL_COUNT; ++i) {
		A->symbol_index[i] = -1;
	}
	
	for(int i = 0; i < lenSigma; ++i) {
		char c = (*Sigma)[i];
		if(c < FIRST_SYMBOL || c > LAST_SYMBOL) {
			fprintf(stderr, "dfa_construct(): symbole non supporté: '%1$c' (%1$i)\n", c);
			exit(1);
		}
		
		A->symbol_index[c - FIRST_SYMBOL] = i;
	}
	
	A->delta = checked_malloc((Q + 1) * sizeof(int*));
	for(int q = 0; q <= Q; ++q) {
		A->delta[q] = checked_malloc(lenSigma * sizeof(int));
		
		for(int s = 0; s < lenSigma; ++s) {
			A->delta[q][s] = INVALID_STATE;
		}
	}
	
	*F = NULL;
	*Sigma = NULL;
	return A;
}

Dfa dfa_parse(char *path) {
	char *rpath = concat("resources/", path);
	FILE *f = fopen(rpath, "r");
	if(f == NULL) {
		fprintf(stderr, "fichier inaccessible: %s\n", path);
		exit(1);
	}
	
	char *buf = NULL;
	size_t bufCapacity;
	size_t line = 0;
	
	int Q = fparse_int(f, path, &line, &buf, &bufCapacity, 0);
	int q0 = fparse_int(f, path, &line, &buf, &bufCapacity, 0);
	
	int lenF;
	int *F = fparse_int_set(f, path, &line, &buf, &bufCapacity, &lenF, Q);
	
	int lenSigma;
	char *Sigma;
	if((lenSigma = getline(&buf, &bufCapacity, f)) != -1) {
		Sigma = malloc(--lenSigma);
		memcpy(Sigma, buf, lenSigma);
	}
	
	Dfa A = dfa_construct(Q, q0, &F, lenF, &Sigma, lenSigma);
	
	int q1 = -1, q2 = -1;
	char c = '\0';
	while(fparse_transition(f, path, &line, &buf, &bufCapacity, &q1, &c, &q2)) {
		dfa_add_transition(A, q1, c, q2);
	}
	
	fclose(f);
	free(rpath);
	free(buf);
	return A;
}

void dfa_add_transition(Dfa A, int q1, char c, int q2) {
	check_param("q1", q1 >= 0 && q1 <= A->Q);
	check_param("c", c >= FIRST_SYMBOL && c <= LAST_SYMBOL);
	check_param("q2", q2 >= 0 && q2 <= A->Q);
	
	A->delta[q1][A->symbol_index[c - FIRST_SYMBOL]] = q2;
}

void dfa_print(Dfa A){
  printf("Q = {0,..,%d}\n", A->Q);
  printf("q0 = %d\n", A->q0);
  printf("F = {");
  for (int i=0; i<A->lenF; i++) printf("%d,",A->F[i]);
  printf("\b}\n");
  int cellsize = (int)(ceil(log10( (double)A->Q)))+1;
  int first_column_size = cellsize>=5 ? cellsize+2 : 7;
  int padding = (cellsize>=5)? (cellsize-5)/2+1: 1;
  int line_length = first_column_size+1+(cellsize+2)*A->lenSigma;
  char * line = malloc(sizeof(char)*(line_length+2));
  for (int i=0; i<=line_length; i++) line[i]='-';
  line[line_length+1]='\0';

  printf("%s\n",line);
  printf("|%*sdelta |", padding, "");
  for(int i=0; i<A->lenSigma; i++) printf("%*c |", cellsize, A->Sigma[i]);
  printf("\n");
  printf("%s\n",line);
  

  for (int q=0; q<A->Q+1; q++){
    printf("|%*d |", padding+5, q);
    for (int i=0; i<A->lenSigma; i++){
      int s = A->symbol_index[A->Sigma[i]-FIRST_SYMBOL];
      if (A->delta[q][s] !=-1)
	printf("%*d |", cellsize, A->delta[q][s]);
      else
	printf("%*s |", cellsize, "");
    }    
    printf("\n");
    printf("%s\n",line);
  }
  free(line);
}

void dfa_free(Dfa *A) {
	free((*A)->F);
	free((*A)->Sigma);
	
	for(int q = 0; q <= (*A)->Q; ++q) {
		free((*A)->delta[q]);
	}
	free((*A)->delta);
	free(*A);
	A = NULL;
}
