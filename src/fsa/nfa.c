#include "fsa/nfa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	check_param("q1", q1 >= 0 && q1 <= A->Q);
	check_param("c", c >= FIRST_SYMBOL && c <= LAST_SYMBOL);
	check_param("q2", q2 >= 0 && q2 <= A->Q);
	
	int s = A->symbol_index[c - FIRST_SYMBOL];
	int **transitions = &(A->Delta[q1][s]);
	
	if(*transitions == NULL) {
		*transitions = malloc(2 * sizeof(int));
		(*transitions)[0] = q2;
		(*transitions)[1] = INVALID_STATE;
	}
	else {
		int count = 0;
		for(int i = 0; (*transitions)[i] != INVALID_STATE; ++i) {
			++count;
		}
		
		int *new_transitions = malloc((count + 2) * sizeof(int));
		memcpy(new_transitions, *transitions, count * sizeof(int));
		new_transitions[count] = q2;
		new_transitions[count + 1] = INVALID_STATE;
		
		free(*transitions);
		*transitions = new_transitions;
	}
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

void nfa_dot(Nfa A, const char *path) {
	char *tmp = concat("out/", path);
	char *out = concat(tmp, ".gz");
	
	free(tmp);
	FILE *f = fopen(out, "w");
	
	fprintf(f,
		"digraph {\n"
		"\tlayout = dot\n"
		"\trankdir = LR\n"
		"\t\n"
		"\tnode [shape=circle]\n"
		"\tedge [arrowhead=open]\n"
		"\t\n"
	);
	
	for(int i = 0; i < A->lenI; ++i) {
		fprintf(f, "\tstart%i [label=\"\", shape=none, height=0, width=0]\n", A->I[i]);
		fprintf(f, "\tstart%1$i -> %1$i\n", A->I[i]);
	}
	
	fprintf(f, "\t\n");
	
	for(int i = 0; i < A->lenF; ++i) {
		fprintf(f, "\t%i [shape=doublecircle]\n", A->F[i]);
	}
	
	fprintf(f, "\t\n");
	
	for(int q = 0; q <= A->Q; ++q) {
		for(int i = 0; i < A->lenSigma; ++i) {
			char c = A->Sigma[i];
			int s = A->symbol_index[c - FIRST_SYMBOL];
			
			if(A->Delta[q][s] != NULL) {
				int r;
				for(int j = 0; (r = A->Delta[q][s][j]) != INVALID_STATE; ++j) {
					fprintf(f, "\t%i -> %i [label=\"%c\"]\n", q, r, c);
				}
			}
		}
	}
	
	fprintf(f, "}\n");
	fclose(f);
	
	char *in = out;
	out = concat(path, ".png");
	
	char command[128];
	snprintf(command, 128, "dot -Tpng %s -o %s", in, out);
	
	int retcode = system(command);
	if(retcode != 0) {
		fprintf(stderr, "dot: system(): %i\n", retcode);
		exit(1);
	}
	
	free(in);
	free(out);
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
