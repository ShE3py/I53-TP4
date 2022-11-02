#include "af.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Initialise le dictionnaire passé en paramètre de telle manière que pour tout caractère
 * ASCII_FIRST <= c <= ASCII_LAST, `dico[c - ASCII_FIRST]` corresponde à l'indice du caractère `c` dans
 * la chaîne `Sigma`, ou à `-1` si `c` n'est pas présent dans `Sigma`.
 *
 * Vérifie aussi qu'aucun caractère n'est dupliqué dans `Sigma`, et que tout caractère dans `Sigma` appartient à
 * l'intervalle [ASCII_FIRST, ASCII_LAST].
 */
void af_init_dico(int dico[MAX_SYMBOLES], const char *Sigma, int lenSigma) {
	for(int i = 0; i < MAX_SYMBOLES; ++i) {
		dico[i] = -1;
	}
	
	for(int i = 0; i < lenSigma; ++i) {
		char c = Sigma[i];
		if(c < ASCII_FIRST || c > ASCII_LAST) {
			fprintf(stderr, "af_init_dico(): symbole non supporté: '%1$c' (%1$i) dans l'alphabet \"%2$s\"\n", c, Sigma);
			exit(1);
		}
		
		int offset = c - ASCII_FIRST;
		if(dico[offset] != -1) {
			fprintf(stderr, "af_init_dico(): symbole dupliqué: '%c' dans l'alphabet \"%s\"\n", c, Sigma);
			exit(1);
		}
		
		dico[offset] = i;
	}
}
