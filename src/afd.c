#include "afd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "util/misc.h"

/**
 * Initialise et renvoie un nouvel AFD à partir de sa définition sans effectuer de copie.
 */
AFD afd_init_owned(int Q, int q0, int *F, int lenF, char *Sigma, int lenSigma) {
	check_param("Q", Q >= 0);
	check_param("q0", q0 >= 0 && q0 <= Q);
	check_param("F", F != NULL);
	check_param("lenF", lenF > 0);
	check_param("Sigma", Sigma != NULL);
	check_param("lenSigma", lenSigma > 0);
	
	AFD A = checked_malloc(sizeof(struct AFD));
	A->Q = Q;
	A->q0 = q0;
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
			fprintf(stderr, "afd_init_owned(): symbole non supporté: '%1$c' (%1$i)\n", c);
			exit(1);
		}
		
		int offset = c - ASCII_FIRST;
		if(A->dico[offset] != -1) {
			fprintf(stderr, "afd_init_owned(): symbole dupliqué: '%c' dans l'alphabet \"%s\"", c, Sigma);
			exit(1);
		}
		
		A->dico[offset] = i;
	}
	
	A->delta = checked_malloc((Q + 1) * sizeof(int*));
	for(int q = 0; q <= Q; ++q) {
		A->delta[q] = checked_malloc(lenSigma * sizeof(int));
		
		for(int s = 0; s < lenSigma; ++s) {
			A->delta[q][s] = INVALID_STATE;
		}
	}
	
	return A;
}

AFD afd_init(int Q, int q0, int nbFinals, int *listFinals, char *Sigma) {
	check_param("nbFinals", nbFinals > 0);
	check_param("listFinals", listFinals != NULL);
	check_param("Sigma", Sigma != NULL);
	
	int lenSigma = strlen(Sigma);
	check_param("strlen(Sigma)", lenSigma > 0);
	
	int *F = checked_malloc(nbFinals * sizeof(int));
	memcpy(F, listFinals, nbFinals * sizeof(int));
	
	char *S = checked_malloc(lenSigma + 1);
	memcpy(S, Sigma, lenSigma + 1);
	
	return afd_init_owned(Q, q0, F, nbFinals, S, lenSigma);
}

void afd_ajouter_transition(AFD A, int q1, char s, int q2) {
	check_param("q1", q1 >= 0 && q1 <= A->Q);
	check_param("s", s >= ASCII_FIRST && s <= ASCII_LAST);
	check_param("q2", q2 >= 0 && q2 <= A->Q);
	
	A->delta[q1][A->dico[s - ASCII_FIRST]] = q2;
}

AFD afd_finit(char *filename) {
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
	int q0 = fparse_int(f, filename, &line, &buf, &bufCapacity, 0);
	
	int lenF;
	int *F = fparse_int_set(f, filename, &line, &buf, &bufCapacity, &lenF, Q);
	
	int lenSigma;
	char *Sigma;
	if((lenSigma = getline(&buf, &bufCapacity, f)) != -1) {
		Sigma = checked_malloc(--lenSigma);
		memcpy(Sigma, buf, lenSigma);
	}
	else {
		fprintf(stderr, "%s: fin du fichier, alphabet attendu\n", filename);
		exit(1);
	}
	
	AFD A = afd_init_owned(Q, q0, F, lenF, Sigma, lenSigma);
	
	int q1 = -1, q2 = -1;
	char c = '\0';
	while(fparse_transition(f, filename, &line, &buf, &bufCapacity, &q1, &c, &q2)) {
		afd_ajouter_transition(A, q1, c, q2);
	}
	
	fclose(f);
	free(rpath);
	free(buf);
	return A;
}


int afd_simuler(AFD A, const char *s) {
	int q = A->q0;
	
	char c;
	for(int i = 0; (c = s[i]) != '\0'; ++i) {
		if(c < ASCII_FIRST || c > ASCII_LAST) {
			return 0;
		}
		
		int s = A->dico[c - ASCII_FIRST];
		if(s == -1) {
			return 0;
		}
		
		q = A->delta[q][s];
	}
	
	for(int i = 0; i < A->lenF; ++i) {
		if(A->F[i] == q) {
			return 1;
		}
	}
	
	return 0;
}

void afd_print(AFD A){
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
      int s = A->dico[A->Sigma[i]-ASCII_FIRST];
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

void afd_free(AFD A) {
	free(A->F);
	free(A->Sigma);
	
	for(int q = 0; q <= A->Q; ++q) {
		free(A->delta[q]);
	}
	
	free(A->delta);
	free(A);
}
