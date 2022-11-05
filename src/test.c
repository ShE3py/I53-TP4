#include <stdio.h>

#include "afd.h"
#include "afn.h"
#include "compregex.h"

#define print(expr)  \
printf(#expr ";\n"); \
expr

#define SIMUL_FUNC afn_simuler

#define assert_simul(A, s, v, _what) \
if(SIMUL_FUNC (A, s) != v) { \
	fprintf(stderr, "assert failed: \"" s "\" is not " _what " by " #A "\n"); \
} \
else {\
	printf("assert ok: \"" s "\" is " _what " by " #A "\n"); \
}

#define assert_accepted(A, s) assert_simul(A, s, 1, "accepted")
#define assert_rejected(A, s) assert_simul(A, s, 0, "rejected")

int main(int argc, char *argv[]) {
	// `sample1.afn`: accepte toutes les chaînes d'au moins un caractère contenant uniquement des `a` ou uniquement des `b`.
	print(AFN A = afn_finit("sample1.afn"));
	print(afn_dot(A, "A"));
	
	assert_rejected(A, "");
	assert_accepted(A, "a");
	assert_accepted(A, "aa");
	assert_accepted(A, "b");
	assert_accepted(A, "bb");
	assert_rejected(A, "ab");
	assert_rejected(A, "ba");
	assert_rejected(A, "c");
	assert_rejected(A, "ac");
	assert_rejected(A, "bc");
	assert_rejected(A, "cca");
	printf("\n");
	
	// `sample2.afn`: accepte toutes les chaînes dans `ab` si les ε-transitions sont bien suivies.
	print(AFN B = afn_finit("sample2.afn"));
	print(afn_dot(B, "B"));
	
	assert_accepted(B, "");
	assert_accepted(B, "a");
	assert_accepted(B, "b");
	assert_accepted(B, "abbabbaaaabab");
	assert_rejected(B, "c");
	printf("\n");
	
	// `sample3.afd`: accepte tous les nombres binaires divisibles par deux.
	print(AFD C = afd_finit("sample3.afd"));

#undef SIMUL_FUNC
#define SIMUL_FUNC afd_simuler
	assert_rejected(C, "");
	assert_accepted(C, "0");
	assert_rejected(C, "1");
	assert_accepted(C, "00");
	assert_accepted(C, "10");
	assert_rejected(C, "01");
	assert_rejected(C, "101");
	assert_accepted(C, "1101010");
	printf("\n");
	
	// `sample4.afd`: accepte toutes les chaînes contenant une seule note de musique possédant un bémole sur un clavier.
	print(AFD D = afd_finit("sample4.afd"));
	
	assert_rejected(D, "");
	assert_accepted(D, "D");
	assert_rejected(D, "F");
	assert_rejected(D, "ED");
	assert_rejected(D, "FG");
	printf("\n");
	
	// test visuel de l'union
	print(AFN E = afn_union(A, B));
	print(afn_dot(E, "E"));
	printf("\n");
	
	// test visuel de la concaténation
	print(AFN F = afn_concat(A, B));
	print(afn_dot(F, "F"));
	printf("\n");
	
	// test visuel de l'étoile de Klenne
	print(AFN G = afn_kleene(A));
	print(afn_dot(G, "G"));
	printf("\n");
	
	afn_free(A);
	afn_free(B);
	afd_free(C);
	afd_free(D);
	afn_free(E);
	afn_free(F);
	afn_free(G);
	
	// test visual d'une ER
	//print(AFN regex = compile("((a.b)+c)*"));
	
}
