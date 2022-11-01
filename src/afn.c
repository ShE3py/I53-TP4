#include "afn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "util/stack.h"
#include "util/misc.h"

/**
 * Initialise et renvoie un nouvel AFN à partir de sa définition sans effectuer de copie.
 */
AFN afn_init_owned(int Q, int *I, int lenI, int *F, int lenF, char *Sigma, int lenSigma) {
	check_param("Q", Q >= 0);
	check_param("I", I != NULL);
	check_param("lenI", lenI > 0);
	check_param("F", F != NULL);
	check_param("lenF", lenF > 0);
	check_param("Sigma", Sigma != NULL);
	check_param("lenSigma", lenSigma > 0);
	
	int i;
	for(i = 0; i < lenSigma; ++i) {
		if(Sigma[i] == EPSILON) {
			break;
		}
	}
	
	if(i == lenSigma) {
		fprintf(stderr, "l'alphabet d'un AFN doit obligatoirement contenir EPSILON\n");
		exit(1);
	}
	
	AFN A = checked_malloc(sizeof(struct AFN));
	A->Q = Q;
	A->I = I;
	A->lenI = lenI;
	A->F = F;
	A->lenF = lenF;
	A->Sigma = Sigma;
	A->lenSigma = lenSigma;
	
	for(int i = 0; i < MAX_SYMBOLES; ++i) {
		A->dico[i] = -1;
	}
	
	for(int i = 0; i < lenSigma; ++i) {
		char c = Sigma[i];
		if(c < ASCII_FIRST || c > ASCII_LAST) {
			fprintf(stderr, "afn_init_owned(): symbole non supporté: '%1$c' (%1$i)\n", c);
			exit(1);
		}
		
		A->dico[c - ASCII_FIRST] = i;
	}
	
	A->delta = checked_malloc((Q + 1) * sizeof(int**));
	for(int q = 0; q <= Q; ++q) {
		A->delta[q] = checked_malloc(lenSigma * sizeof(int*));
		
		for(int s = 0; s < lenSigma; ++s) {
			A->delta[q][s] = NULL;
		}
	}
	
	return A;
}

AFN afn_init(int Q, int nbInitiaux, int *listInitiaux, int nbFinals, int *listFinals, char *Sigma) {
	check_param("nbInitiaux", nbInitiaux > 0);
	check_param("listInitiaux", listInitiaux != NULL);
	check_param("nbFinals", nbFinals > 0);
	check_param("listFinals", listFinals != NULL);
	check_param("Sigma", Sigma != NULL);
	
	int lenSigma = strlen(Sigma);
	check_param("strlen(Sigma)", lenSigma > 0);
	
	int *I = checked_malloc(nbInitiaux * sizeof(int));
	memcpy(I, listInitiaux, nbInitiaux * sizeof(int));
	
	int *F = checked_malloc(nbFinals * sizeof(int));
	memcpy(F, listFinals, nbFinals * sizeof(int));
	
	int i;
	for(i = 0; i < lenSigma; ++i) {
		if(Sigma[i] == EPSILON) {
			break;
		}
	}
	
	char *S;
	if(i == lenSigma) {
		++lenSigma;
		
		S = checked_malloc(lenSigma + 1);
		memcpy(S + 1, Sigma, lenSigma);
		S[0] = EPSILON;
	}
	else {
		S = checked_malloc(lenSigma + 1);
		memcpy(S, Sigma, lenSigma + 1);
	}
	
	return afn_init_owned(Q, I, nbInitiaux, F, nbFinals, S, lenSigma);
}

void afn_ajouter_transition(AFN A, int q1, char c, int q2) {
	check_param("q1", q1 >= 0 && q1 <= A->Q);
	check_param("c", c >= ASCII_FIRST && c <= ASCII_LAST);
	check_param("q2", q2 >= 0 && q2 <= A->Q);
	
	int s = A->dico[c - ASCII_FIRST];
	int **transitions = &(A->delta[q1][s]);
	
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

AFN afn_finit(char *filename) {
	char *rpath = concat("resources/", filename);
	FILE *f = fopen(rpath, "r");
	if(f == NULL) {
		fprintf(stderr, "fichier inaccessible: %s\n", filename);
		exit(1);
	}
	
	char *buf = NULL;
	size_t bufCapacity;
	size_t line = 0;
	
	int Q = fparse_int(f, filename, &line, &buf, &bufCapacity, 0);
	
	int lenI;
	int *I = fparse_int_set(f, filename, &line, &buf, &bufCapacity, &lenI, Q);
	
	int lenF;
	int *F = fparse_int_set(f, filename, &line, &buf, &bufCapacity, &lenF, Q);
	
	int lenSigma;
	char *Sigma;
	if((lenSigma = getline(&buf, &bufCapacity, f)) != -1) {
		Sigma = checked_malloc(lenSigma);
		memcpy(Sigma + 1, buf, lenSigma - 1);
		Sigma[0] = EPSILON;
	}
	else {
		fprintf(stderr, "%s: fin du fichier, alphabet attendu\n", filename);
		exit(1);
	}
	
	AFN A = afn_init_owned(Q, I, lenI, F, lenF, Sigma, lenSigma);
	
	int q1 = -1, q2 = -1;
	char c = '\0';
	while(fparse_transition(f, filename, &line, &buf, &bufCapacity, &q1, &c, &q2)) {
		afn_ajouter_transition(A, q1, c, q2);
	}
	
	fclose(f);
	free(rpath);
	free(buf);
	return A;
}

void nfa_epsilon_closure_in_place(AFN A, set *G) {
	stack s = stack_init_from(G->buf, G->len);
	
	while(!stack_is_empty(s)) {
		int q = stack_pop(&s);
		
		int *q1 = A->delta[q][A->dico[EPSILON - ASCII_FIRST]];
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

int* afn_epsilon_fermeture(AFN A, int *R) {
	stack s = stack_init();
	if(R != NULL) {
		while(*R != INVALID_STATE) {
			stack_push(&s, *R);
			++R;
		}
	}
	
	// assume R is sorted
	set S;
	S.buf = s.buf;
	S.len = s.len;
	S.capacity = s.capacity;
	
	nfa_epsilon_closure_in_place(A, &S);
	
	// append INVALID_STATE
	s.buf = S.buf;
	s.len = S.len;
	s.capacity = S.capacity;
	stack_push(&s, INVALID_STATE);
	
	return s.buf;
}

int afn_simuler(AFN A, const char *s) {
	set R = set_init_from(A->I, A->lenI);
	nfa_epsilon_closure_in_place(A, &R);
	
	char c;
	for(size_t i = 0; (c = s[i]) != '\0'; ++i) {
		set tmp = set_init();
		int s = A->dico[c - ASCII_FIRST];
		
		for(size_t j = 0; j < R.len; ++j) {
			int q = R.buf[j];
			int *q1 = A->delta[q][s];
			
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

void afn_print(AFN A) {
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
      if (A->delta[q][s]!=NULL){
	int cell_size = 0;
	
	while (A->delta[q][s][cell_size]!=-1) cell_size++;
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
      int s = A->dico[A->Sigma[i]-ASCII_FIRST];
      if (A->delta[q][s] != NULL){
	int j=0;
	buffer[0]='{';
	buffer[1]='\0';
	while (A->delta[q][s][j]!=-1) {
	  sprintf(buffer,"%s%d,",buffer, A->delta[q][s][j++]);
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

void afn_dot(AFN A, const char *path) {
	char *tmp = concat("out/", path);
	char *out = concat(tmp, ".gv");
	
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
			int s = A->dico[c - ASCII_FIRST];
			
			if(A->delta[q][s] != NULL) {
				int r;
				for(int j = 0; (r = A->delta[q][s][j]) != INVALID_STATE; ++j) {
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

void afn_free(AFN A) {
	free(A->I);
	free(A->F);
	free(A->Sigma);
	
	for(int q = 0; q <= A->Q; ++q) {
		for(int s = 0; s < A->lenSigma; ++s) {
			free(A->delta[q][s]);
		}
		
		free(A->delta[q]);
	}
	
	free(A->delta);
	free(A);
	A = NULL;
}
