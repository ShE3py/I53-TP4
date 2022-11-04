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
/**
 * Initialise et renvoie un nouvel AFN à partir de sa définition sans effectuer de copie.
 *
 * Paramètres:
 * - Q     : le plus grand état
 * - I     : un tableau des états initiaux
 * - lenI  : le nombre d'états initiaux
 * - F     : un tableau des états finaux
 * - lenF  : le nombre d'états finaux
 * - Sigma : une chaîne de caractères terminée par '\0' qui représentera l'alphabet
 *
 * Voir aussi:
 * - `afn_init(int, int, int*, int, int*, char*)`
 * - `afn_ajouter_transition(AFN, int, char, int)`
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
	
	af_init_dico(A->dico, Sigma, lenSigma);
	
	A->delta = checked_malloc((Q + 1) * sizeof(int**));
	for(int q = 0; q <= Q; ++q) {
		A->delta[q] = checked_malloc(lenSigma * sizeof(int*));
		
		for(int s = 0; s < lenSigma; ++s) {
			A->delta[q][s] = NULL;
		}
	}
	
	return A;
}


/**
 * Initialise et renvoie un nouvel AFN à partir de sa définition.
 *
 * Paramètres:
 * - Q            : le plus grand état
 * - nbInitiaux   : le nombre d'états initiaux
 * - listInitiaux : un tableau des états initiaux
 * - nbFinals     : le nombre d'états finaux
 * - listFinals   : un tableau des états finaux
 * - Sigma        : une chaîne de caractères terminée par '\0' qui représentera l'alphabet
 *
 * Voir aussi:
 * - `afn_ajouter_transition(AFN, int, char, int)`
 */
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
		// ajout d'EPSILON dans Sigma si celui-ci n'est pas déjà présent
		++lenSigma;
		
		S = checked_malloc(lenSigma + 1);
		memcpy(S + 1, Sigma, lenSigma);
		S[0] = EPSILON;
	}
	else {
		// sinon copie simple avec le '\0'
		S = checked_malloc(lenSigma + 1);
		memcpy(S, Sigma, lenSigma + 1);
	}
	
	return afn_init_owned(Q, I, nbInitiaux, F, nbFinals, S, lenSigma);
}


/**
 * Modifie la fonction de transition de l'AFN spécifié de façon à ce que Δ(q1, s) contienne l'état q2.
 */
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


/**
 * Initialise et renvoie un nouvel AFN à partir d'un fichier `filename` écrit au format :
 * ```
 * Q
 * lenI
 * I[0] I[1] ... I[lenI - 1]
 * lenF
 * F[0] F[1] ... F[lenF - 1]
 * Sigma
 * q0 τ0 q'0
 * q1 τ1 q'1
 * ...
 * qk τk q'k
 * ```
 */
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
	char *Sigma = fparse_Sigma(f, filename, &line, &buf, &bufCapacity, &lenSigma);
	
	// ajout d'EPSILON dans Sigma
	++lenSigma;
	
	char *S = checked_malloc(lenSigma + 1);
	memcpy(S + 1, Sigma, lenSigma);
	S[0] = EPSILON;
	
	free(Sigma);
	Sigma = S;
	
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


/**
 * Calcul l'epsilon-fermeture d'un ensemble d'états `G` passé en paramètre; écrit ce résultat dans cedit paramètre.
 */
void afn_epsilon_closure_assign(AFN A, set *G) {
	stack accessible = stack_copy_from(G->buf, G->len);
	
	while(!stack_is_empty(accessible)) {
		int q = stack_pop(&accessible);
		
		int *q2 = A->delta[q][A->dico[EPSILON - ASCII_FIRST]];
		if(q2 == NULL) {
			continue;
		}
		
		while(*q2 != INVALID_STATE) {
			if(set_push(G, *q2)) {
				stack_push(&accessible, *q2);
			}
			
			++q2;
		}
	}
	
	stack_free(&accessible);
}


/**
 * Calcul et renvoie l'epsilon-fermeture d'un ensemble d'états `R` trié par ordre croissant et dont le dernier élément `INVALID_STATE`.
 */
int* afn_epsilon_fermeture(AFN A, int *R) {
	// copie de `R` dans une pile
	stack s = stack_new_empty();
	if(R != NULL) {
		while(*R != INVALID_STATE) {
			stack_push(&s, *R);
			++R;
		}
	}
	
	// conversion de la pile en un ensemble
	// SAFETY: l'appellant doit s'assurer que `R` soit trié et sans doublon
	set S;
	S.buf = s.buf;
	S.len = s.len;
	S.capacity = s.capacity;
	
	// calcul de l'epsilon-fermeture
	afn_epsilon_closure_assign(A, &S);
	
	// reconversion en pile, puis ajout de `INVALID_STATE`
	s.buf = S.buf;
	s.len = S.len;
	s.capacity = S.capacity;
	stack_push(&s, INVALID_STATE);
	
	// l'appelant doit s'assurer de `free()` l'adresse retournée
	return s.buf;
}


/**
 * Renvoie `1` si la chaîne spécifiée est acceptée par l'AFN spécifié, sinon renvoie `0`.
 */
int afn_simuler(AFN A, const char *s) {
	// `R` contient tous les états dans lesquels l'AFN peut virtuellement être,
	// on commence avec les états initiaux
	set R = set_copy_from(A->I, A->lenI);
	
	char c;
	for(size_t i = 0; (c = s[i]) != '\0'; ++i) {
		// vérification des caractères qui ne seront jamais acceptés par un AFN
		if(c < ASCII_FIRST || c > ASCII_LAST || c == EPSILON) {
			return 0;
		}
		
		// chaque caractère `c` lu dans `s` doit modifier `R`,
		// mais il faut aussi prendre en compte les epsilon-transitions
		afn_epsilon_closure_assign(A, &R);
		
		set R_next = set_new_empty();
		int s = A->dico[c - ASCII_FIRST];
		
		// vérification que le caractère soit bien dans l'alphabet de l'AF
		if(s == -1) {
			return 0;
		}
		
		for(size_t j = 0; j < R.len; ++j) {
			// pour toutes les transitions possibles en lisant `c` dans un état particulier de `R`,
			int *q2 = A->delta[R.buf[j]][s];
			
			if(q2 != NULL) {
				// on converti la liste des transitions possibles en un ensemble,
				set s = set_new_empty();
				while(*q2 != INVALID_STATE) {
					set_push(&s, *q2);
					++q2;
				}
				
				// et on ajoute les états accessibles dans `R_next`
				set _union = set_union(R_next, s);
				
				set_free(&R_next);
				set_free(&s);
				R_next = _union;
			}
		}
		
		// on a fini de traité le caractère lu, on passe aux états suivants
		set_free(&R);
		R = R_next;
	}
	
	// `s` appartient à l'AFN si et seulement si un de nos états `R` appartient aux états finaux
	set F = set_copy_from(A->F, A->lenF);
	int accepted = set_are_intersecting(R, F);
	
	set_free(&R);
	set_free(&F);
	return accepted;
}


/**
 * Affiche l'AFN spécifié dans le flux de sortie standard.
 */
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


/**
 * Dessine un AFN dans un fichier `filename.png`.
 */
void afn_dot(AFN A, const char *path) {
	char *tmp = concat("out/", path);
	char *out = concat(tmp, ".gv");
	
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
	
	out = concat(tmp, ".png");
	free(tmp);
	
	char command[128];
	snprintf(command, 128, "dot -Tpng %s -o %s", in, out);
	
	int retcode = system(command);
	if(retcode != 0) {
		fprintf(stderr, "dot: system(): %i\n", retcode);
//		exit(1);
	}
	
	free(in);
	free(out);
}


/**
 * Libère les ressources allouées à un AFN.
 */
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
