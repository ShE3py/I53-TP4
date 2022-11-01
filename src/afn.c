#include "fsa/nfa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fsa/mod.h"
#include "util/stack.h"
#include "util/misc.h"

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
		fprintf(stderr, "l'alphabet d'un AFN doit contenir EPSILON\n");
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
		char c = (*Sigma)[i];
		if(c < FIRST_SYMBOL || c > LAST_SYMBOL) {
			fprintf(stderr, "nfa_construct(): symbole non supporté: '%1$c' (%1$i)\n", c);
			exit(1);
		}
		
		A->symbol_index[c - FIRST_SYMBOL] = i;
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

Nfa nfa_parse(char *path) {
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
	
	int lenI;
	int *I = fparse_int_set(f, path, &line, &buf, &bufCapacity, &lenI, Q);
	
	int lenF;
	int *F = fparse_int_set(f, path, &line, &buf, &bufCapacity, &lenF, Q);
	
	int lenSigma;
	char *Sigma;
	if((lenSigma = getline(&buf, &bufCapacity, f)) != -1) {
		Sigma = checked_malloc(lenSigma);
		memcpy(Sigma + 1, buf, lenSigma - 1);
		Sigma[0] = EPSILON;
	}
	
	Nfa A = nfa_construct(Q, &I, lenI, &F, lenF, &Sigma, lenSigma);
	
	int q1 = -1, q2 = -1;
	char c = '\0';
	while(fparse_transition(f, path, &line, &buf, &bufCapacity, &q1, &c, &q2)) {
		nfa_add_transition(A, q1, c, q2);
	}
	
	fclose(f);
	free(rpath);
	free(buf);
	return A;
}

void nfa_add_transition(Nfa A, int q1, char c, int q2) {
	check_param("q1", q1 >= 0 && q1 <= A->Q);
	check_param("c", c >= FIRST_SYMBOL && c <= LAST_SYMBOL);
	check_param("q2", q2 >= 0 && q2 <= A->Q);
	
	int s = A->symbol_index[c - FIRST_SYMBOL];
	int **transitions = &(A->Delta[q1][s]);
	
	if(*transitions == NULL) {
		*transitions = checked_malloc(2 * sizeof(int));
		(*transitions)[0] = q2;
		(*transitions)[1] = INVALID_STATE;
	}
	else {
		int count = 0;
		for(int i = 0; (*transitions)[i] != INVALID_STATE; ++i) {
			++count;
		}
		
		int *new_transitions = checked_malloc((count + 2) * sizeof(int));
		memcpy(new_transitions, *transitions, count * sizeof(int));
		new_transitions[count] = q2;
		new_transitions[count + 1] = INVALID_STATE;
		
		free(*transitions);
		*transitions = new_transitions;
	}
}

void nfa_epsilon_closure_in_place(Nfa A, set *G) {
	stack s = stack_init_from(G->buf, G->len);
	
	while(!stack_is_empty(s)) {
		int q = stack_pop(&s);
		
		int *q1 = A->Delta[q][A->symbol_index[EPSILON - FIRST_SYMBOL]];
		if(q1 == NULL) {
			continue;
		}
		
		while(*q1 != INVALID_STATE) {
			if(set_push(G, *q1)) {
				stack_push(&s, *q1);
			}
			
			++q1;
		}
	}
	
	stack_free(&s);
}

int nfa_is_accepted(Nfa A, const char *s) {
	set R = set_init_from(A->I, A->lenI);
	nfa_epsilon_closure_in_place(A, &R);
	
	char c;
	for(size_t i = 0; (c = s[i]) != '\0'; ++i) {
		set tmp = set_init();
		int s = A->symbol_index[c - FIRST_SYMBOL];
		
		for(size_t j = 0; j < R.len; ++j) {
			int q = R.buf[j];
			int *q1 = A->Delta[q][s];
			
			if(q1 != NULL) {
				set q1s = set_init();
				while(*q1 != INVALID_STATE) {
					set_push(&q1s, *q1);
					++q1;
				}
				
				set tmp2 = set_union(tmp, q1s);
				
				set_free(&tmp);
				set_free(&q1s);
				tmp = tmp2;
			}
		}
		
		set_free(&R);
		R = tmp;
	}
	
	set F = set_init_from(A->F, A->lenF);
	int accepted = !set_are_disjoints(R, F);
	
	set_free(&R);
	set_free(&F);
	return accepted;
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
	
	tmp = concat("resources/", path);
	out = concat(tmp, ".png");
	free(tmp);
	
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
