#include "compat.h"

#include <stdlib.h>
#include <string.h>

#include "fsa/mod.h"
#include "set.h"
#include "stack.h"
#include "misc.h"

AFD afd_init(int Q, int q0, int nbFinals, int *listFinals, char *Sigma) {
	check_param("nbFinals", nbFinals > 0);
	check_param("listFinals", listFinals != NULL);
	check_param("Sigma", Sigma != NULL);
	
	int lenSigma = strlen(Sigma);
	check_param("strlen(Sigma)", lenSigma > 0);
	
	int *F = checked_malloc(nbFinals * sizeof(int));
	memcpy(F, listFinals, nbFinals * sizeof(int));
	
	char *S = checked_malloc(lenSigma);
	memcpy(S, Sigma, lenSigma);
	
	return dfa_construct(Q, q0, &F, nbFinals, &S, lenSigma);
}

AFD afd_finit(char *file) {
	return dfa_parse(file);
}

void afd_ajouter_transition(AFD A, int q1, char s, int q2) {
	dfa_add_transition(A, q1, s, q2);
}

int afd_simuler(AFD A, char *s) {
	return dfa_is_accepted(A, s);
}

void afd_print(AFD A) {
	dfa_print(A);
}

void afd_free(AFD A) {
	dfa_free(&A);
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
		
		S = checked_malloc(lenSigma);
		memcpy(S + 1, Sigma, lenSigma - 1);
		S[0] = EPSILON;
	}
	else {
		S = checked_malloc(lenSigma);
		memcpy(S, Sigma, lenSigma);
	}
	
	return nfa_construct(Q, &I, nbInitiaux, &F, nbFinals, &S, lenSigma);
}

AFN afn_finit(char *file) {
	return nfa_parse(file);
}

void afn_ajouter_transition(AFN A, int q1, char s, int q2) {
	nfa_add_transition(A, q1, s, q2);
}

void afn_print(AFN A) {
	nfa_print(A);
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

int afn_simuler(AFN A, char *s) {
	return nfa_is_accepted(A, s);
}

void afn_free(AFN A) {
	nfa_free(&A);
}
