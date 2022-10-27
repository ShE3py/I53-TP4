#include "compat.h"

#include <stdlib.h>
#include <string.h>

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

void afd_ajouter_transition(AFD A, int q1, char s, int q2) {
	dfa_add_transition(A, q1, s, q2);
}

void afd_print(AFD A) {
	dfa_print(A);
}

void afd_free(AFD A) {
	dfa_free(&A);
}
