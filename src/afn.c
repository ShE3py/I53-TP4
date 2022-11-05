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
AFN afn_init(int Q, int nbInitiaux, const int *listInitiaux, int nbFinals, const int *listFinals, const char *Sigma) {
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
	if(s == -1) {
		fprintf(stderr, "afn_ajouter_transition(): '%c' n'appartient pas à \"%s\"\n", c, A->Sigma);
		exit(1);
	}
	
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
AFN afn_finit(const char *filename) {
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
int* afn_epsilon_fermeture(AFN A, const int *R) {
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
	// on commence avec les états initiaux et tous ceux accessibles par epsilon-transitions
	set R = set_copy_from(A->I, A->lenI);
	afn_epsilon_closure_assign(A, &R);
	
	char c;
	for(size_t i = 0; (c = s[i]) != '\0'; ++i) {
		// vérification des caractères qui ne seront jamais acceptés par un AFN
		if(c < ASCII_FIRST || c > ASCII_LAST || c == EPSILON) {
			set_free(&R);
			return 0;
		}
		
		// vérification que le caractère soit bien dans l'alphabet de l'AF
		int s = A->dico[c - ASCII_FIRST];
		
		if(s == -1) {
			set_free(&R);
			return 0;
		}
		
		// chaque caractère `c` lu dans `s` doit modifier `R`,
		// cepedant il faut conserver `R` pour généré le nouveau `R`,
		// d'où une variable temporaire
		
		set R_next = set_new_empty();
		
		for(size_t j = 0; j < R.len; ++j) {
			// pour toutes les transitions possibles en lisant `c` dans un état particulier de `R`,
			int *q2 = A->delta[R.buf[j]][s];
			
			if(q2 != NULL) {
				// on converti la liste des transitions possibles en un ensemble,
				set next = set_new_empty();
				while(*q2 != INVALID_STATE) {
					set_push(&next, *q2);
					++q2;
				}
				
				// et on ajoute les états accessibles dans `R_next`
				set _union = set_union(R_next, next);
				
				set_free(&R_next);
				set_free(&next);
				R_next = _union;
			}
		}
		
		// on a fini de traité le caractère lu, on passe aux états suivants et ceux accessibles par epsilon-transitions
		set_free(&R);
		
		R = R_next;
		afn_epsilon_closure_assign(A, &R);
	}
	
	// `s` appartient à l'AFN si et seulement si un de nos états dans l'epsilon-fermeture de `R` appartient aux états finaux
	// à ce stade, il est garanti que `R` contienne déjà son epsilon-fermeture
	set F = set_copy_from(A->F, A->lenF);
	int accepted = set_are_intersecting(R, F);
	
	set_free(&R);
	set_free(&F);
	return accepted;
}


/**
 * Construit et renvoie un AFN acceptant le langage constitué du seul symbole `c`.
 */
AFN afn_char(char c, const char *Sigma) {
	const int Q = 1;
	const int q0 = 0;
	const int q1 = 1;
	
	AFN A = afn_init(Q, 1, &q0, 1, &q1, Sigma);
	afn_ajouter_transition(A, q0, c, q1);
	
	return A;
}


/**
 * Additionne un nombre à tous les éléments du tableau spécifié.
 *
 * Paramètres:
 * - transitions: un tableau de transitions
 * - len:       : le nombre de transitions
 * - offset     : la valeur à ajouter à tous les éléments du tableau
 *
 * Renvoie: un nouveau tableau terminé par `INVALID_STATE` qu'il faudra désallouer avec `free()`.
 */
int* rshift_all_sized(const int *transitions, size_t len, size_t offset) {
	if(transitions == NULL) {
		return NULL;
	}
	
	int *out = checked_malloc((len + 1) * sizeof(int));
	for(size_t i = 0; i < len; ++i) {
		out[i] = transitions[i] + offset;
	}
	
	out[len] = INVALID_STATE;
	return out;
}


/**
 * Additionne un nombre à tous les éléments du tableau spécifié.
 *
 * Paramètres:
 * - transitions: un tableau de transitions terminé par `INVALID_STATE`.
 * - offset     : la valeur à ajouter à tous les éléments du tableau
 *
 * Renvoie: un nouveau tableau terminé par `INVALID_STATE` qu'il faudra désallouer avec `free()`.
 */
int* rshift_all(const int *transitions, size_t offset) {
	if(transitions == NULL) {
		return NULL;
	}
	
	size_t len = 0;
	const int *ptr = transitions;
	while(*ptr != INVALID_STATE) {
		++len;
		++ptr;
	}
	
	return rshift_all_sized(transitions, len, offset);
}


/**
 * Modifie la fonction de transition `delta` pour prendre en compte toutes les transitions de `A` dont
 * les états auront étés augmentés de `qA_offset`.
 */
void afn_delta_copy_assign(int ***delta, AFN A, int qA_offset) {
	for(int qA = 0; qA <= A->Q; ++qA) {
		for(int s = 0; s < A->lenSigma; ++s) {
			delta[qA + qA_offset][s] = rshift_all(A->delta[qA][s], qA_offset);
		}
	}
}


/**
 * Construit et renvoie l'union de deux AFN.
 */
AFN afn_union(AFN A, AFN B) {
	check_param("A->Sigma equals B->Sigma", strcmp(A->Sigma, B->Sigma) == 0);
	
	const int Q = A->Q + B->Q + 3;
	const int q0 = 0;
	const int qQ = Q;
	const char *Sigma = A->Sigma;
	
	AFN U = afn_init(Q, 1, &q0, 1, &qQ, Sigma);
	
	// Décalage des états de `A` et `B` dans l'union `U`
	const int qA_offset = 1;
	const int qB_offset = A->Q + 2;
	
	// Copie des transitions présentes dans `A` et `B`
	afn_delta_copy_assign(U->delta, A, qA_offset);
	afn_delta_copy_assign(U->delta, B, qB_offset);
	
	// Ajout des ε-transitions de l'état initial `q0` de `U` vers les états initiaux de `A` et `B`
	for(int i = 0; i < A->lenI; ++i) {
		afn_ajouter_transition(U, q0, EPSILON, A->I[i] + qA_offset);
	}
	
	for(int i = 0; i < B->lenI; ++i) {
		afn_ajouter_transition(U, q0, EPSILON, B->I[i] + qB_offset);
	}
	
	// Ajout des ε-transitions des états finaux de `A` et `B` vers l'état final `qQ` de `U`
	for(int i = 0; i < A->lenF; ++i) {
		afn_ajouter_transition(U, A->F[i] + qA_offset, EPSILON, qQ);
	}
	
	for(int i = 0; i < B->lenF; ++i) {
		afn_ajouter_transition(U, B->F[i] + qB_offset, EPSILON, qQ);
	}
	
	return U;
}


/**
 * Construit et renvoie la concaténation de deux AFN.
 */
AFN afn_concat(AFN A, AFN B) {
	check_param("A->Sigma equals B->Sigma", strcmp(A->Sigma, B->Sigma) == 0);
	
	const int Q = A->Q + B->Q + 1;
	const char *Sigma = A->Sigma;
	
	// Aucun décalage pour les états de `A`, un décalage pour ceux de `B` dans la concaténation `C`
	const int qA_offset = 0;
	const int qB_offset = A->Q + 1;
	
	// Décalage des nouveaux états finaux
	int *F = rshift_all_sized(B->F, B->lenF, qB_offset);
	
	AFN C = afn_init(Q, A->lenI, A->I, B->lenF, F, Sigma);
	free(F);
	
	// Copie des transitions présentes dans `A` et `B`
	afn_delta_copy_assign(C->delta, A, qA_offset);
	afn_delta_copy_assign(C->delta, B, qB_offset);
	
	// Ajout des ε-transitions depuis les états finaux de `A` vers les états initiaux de `B`
	for(int i = 0; i < A->lenF; ++i) {
		for(int j = 0; j < B->lenI; ++j) {
			afn_ajouter_transition(C, A->F[i] + qA_offset, EPSILON, B->I[j] + qB_offset);
		}
	}
	
	return C;
}


/**
 * Construit et renvoie l'étoile de Klenne d'un AFN.
 */
AFN afn_kleene(AFN A) {
	const int Q = A->Q + 3;
	const int q0 = 0;
	const int qQ = Q;
	
	AFN K = afn_init(Q, 1, &q0, 1, &qQ, A->Sigma);
	
	// Décalage des états de `A` dans `K`
	const int qA_offset = 1;
	
	// Copie des transitions de `A`
	afn_delta_copy_assign(K->delta, A, qA_offset);
	
	// Ajout des ε-transitions depuis `q0` vers les états initiaux de `A`
	for(int i = 0; i < A->lenI; ++i) {
		afn_ajouter_transition(K, q0, EPSILON, A->I[i] + qA_offset);
	}
	
	// Ajout des ε-transitions depuis les états finaux de `A` vers `qQ`
	for(int i = 0; i < A->lenF; ++i) {
		afn_ajouter_transition(K, A->F[i] + qA_offset, EPSILON, qQ);
	}
	
	// Ajout des ε-transitions depuis les états finaux de `A` vers ses états initiaux
	// (répétabilité)
	for(int i = 0; i < A->lenF; ++i) {
		for(int j = 0; j < A->lenI; ++j) {
			afn_ajouter_transition(K, A->F[i] + qA_offset, EPSILON, A->I[j] + qA_offset);
		}
	}
	
	// Ajout de l'ε-transition depuis `q0` vers `qQ` (optionnalité)
	afn_ajouter_transition(K, q0, EPSILON, qQ);
	
	return K;
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

  char *buffer = malloc(sizeof(char)*(total_cell_size+2));
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
			
			char label[3];
			if(c == EPSILON) {
				label[0] = '\xCE';
				label[1] = '\xB5';
				label[2] = '\0';
			}
			else {
				label[0] = c;
				label[1] = '\0';
			}
			
			if(A->delta[q][s] != NULL) {
				int r;
				for(int j = 0; (r = A->delta[q][s][j]) != INVALID_STATE; ++j) {
					fprintf(f, "\t%i -> %i [label=\"%s\"]\n", q, r, (const char*) &label);
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
