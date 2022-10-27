#include "fsa/nfa.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fsa/mod.h"
#include "misc.h"

Nfa nfa_construct(int Q, int **I, int lenI, int **F, int lenF, char **Sigma, int lenSigma) {
	check_param("Q", Q >= 0);
	check_param("I", I != NULL && *I != NULL);
	check_param("lenI", lenI > 0);
	check_param("F", F != NULL && *F != NULL);
	check_param("lenF", lenF > 0);
	check_param("Sigma", Sigma != NULL && *Sigma != NULL);
	check_param("lenSigma", lenSigma > 0);
	
	int i;
	for(i = 0; i < lenSigma; ++i) {
		if((*Sigma)[i] == EPSILON) {
			break;
		}
	}
	
	if(i == lenSigma) {
		fprintf(stderr, "l'alphabet d'un AFN doit contenir EPSILON");
		exit(1);
	}
	
	Nfa A = checked_malloc(sizeof(struct Nfa));
	A->Q = Q;
	A->I = *I;
	A->lenI = lenI;
	A->F = *F;
	A->lenF = lenF;
	A->Sigma = *Sigma;
	A->lenSigma = lenSigma;
	
	for(int i = 0; i < SYMBOL_COUNT; ++i) {
		A->symbol_index[i] = -1;
	}
	
	for(int i = 0; i < lenSigma; ++i) {
		A->symbol_index[(*Sigma)[i] - FIRST_SYMBOL] = i;
	}
	
	A->Delta = checked_malloc((Q + 1) * sizeof(int**));
	for(int q = 0; q <= Q; ++q) {
		A->Delta[q] = checked_malloc(lenSigma * sizeof(int*));
		
		for(int s = 0; s < lenSigma; ++s) {
			A->Delta[q][s] = NULL;
		}
	}
	
	*I = NULL;
	*F = NULL;
	*Sigma = NULL;
	return A;
}

void nfa_add_transition(Nfa A, int q1, char c, int q2) {
	
}

void nfa_print(Nfa A) {
  printf("Q = {0,..,%d}\n", A->Q);
  printf("I = {");
  for (int i=0; i<A->lenI; i++) printf("%d,",A->I[i]);
  printf("\b}\n");

  printf("F = {");
  for (int i=0; i<A->lenF; i++) printf("%d,",A->F[i]);
  printf("\b}\n");

  int state_size = (int)(ceil(log10( (double)A->Q)));
  int padding = (state_size>=5)? (state_size-5)/2+1: 1;
  int first_column_size = state_size>=5 ? state_size+2 : 7;
  int max_cell_size = 0;

  for (int q=0; q<A->Q; q++){
    for (int s=0; s<A->lenSigma; s++){
      if (A->Delta[q][s]!=NULL){
	int cell_size = 0;
	
	while (A->Delta[q][s][cell_size]!=-1) cell_size++;
	max_cell_size = (max_cell_size < cell_size ? cell_size : max_cell_size);
      }
    }
  }
  int total_cell_size = max_cell_size*(state_size+1)+1;

  int line_length = first_column_size+1+(total_cell_size+1)*A->lenSigma;
  char * line = malloc(sizeof(char)*(line_length+2));
  for (int i=0; i<=line_length; i++) line[i]='-';
  line[line_length+1]='\0';
  printf("%s\n",line);
  printf("|%*sDelta |", padding, "");
  for (int i=0; i<A->lenSigma; i++) printf("%*c |", total_cell_size-1, A->Sigma[i]);
  printf("\n");
  printf("%s\n",line);

  char *buffer = malloc(sizeof(char)*(total_cell_size+1));
  for (int q=0; q<A->Q+1; q++){
    printf("|%*d |", padding+5, q);
    for (int i=0; i<A->lenSigma; i++){
      int s = A->symbol_index[A->Sigma[i]-FIRST_SYMBOL];
      if (A->Delta[q][s] != NULL){
	int j=0;
	buffer[0]='{';
	buffer[1]='\0';
	while (A->Delta[q][s][j]!=-1) {
	  sprintf(buffer,"%s%d,",buffer, A->Delta[q][s][j++]);
	}
	printf("%*s\b}|", total_cell_size ,buffer );
      } else {
	printf("%*s|",total_cell_size,"");
      }
    }
    printf("\n");
    printf("%s\n",line);
  }
  free(buffer);
  free(line);
}

void nfa_free(Nfa *A) {
	free((*A)->I);
	free((*A)->F);
	free((*A)->Sigma);
	
	for(int q = 0; q <= (*A)->Q; ++q) {
		for(int s = 0; s < (*A)->lenSigma; ++s) {
			free((*A)->Delta[q][s]);
		}
		
		free((*A)->Delta[q]);
	}
	
	free((*A)->Delta);
	free(*A);
	A = NULL;
}
