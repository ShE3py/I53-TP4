#ifndef AFN_H
#define AFN_H

#include "af.h"

#include "util/set.h"

/**
 * Le symbole représentant une possible epsilon-transition dans un AFN.
 *
 * Les états sont représentés par des nombres entiers positifs.
 */
#define EPSILON '&'


/**
 * Représente un automate fini non-déterministe (AFN).
 */
struct AFN {
	/**
	 * Le plus grand état de l'automate.
	 *
	 * Tout état q de l'automate respecte la propriété suivante :
	 * 0 <= q <= Q
	 *
	 * L'automate possède exactement Q + 1 états ; en d'autres mots, tous les entiers sont utilisés.
	 */
	int Q;
	
	/**
	 * Les états initiaux de l'automate.
	 */
	int *I;
	
	/**
	 * Le nombre d'états initiaux de l'automate.
	 */
	int lenI;
	
	/**
	 * Les états finaux de l'automate.
	 */
	int *F;
	
	/**
	 * Le nombre d'états finaux de l'automate.
	 */
	int lenF;
	
	/**
	 * L'alphabet de l'automate. La chaîne termine par un caractère nul.
	 */
	char *Sigma;
	
	/**
	 * La taille de l'alphabet de l'automate.
	 */
	int lenSigma;
	
	/**
	 * La fonction de transition de l'automate.
	 * Δ(q, τ) = delta[q][dico[τ - ASCII-FIRST]]
	 *
	 * La fin d'un ensemble dans l'ensemble des parties est marquée par la constante `INVALID_STATE`.
	 * L'ensemble vide peut être noté par un singleton `{ INVALID_STATE }` ou par `NULL`.
	 */
	int ***delta;
	
	/**
	 * Ce tableau permet de récupérer l'indice du symbole τ dans l'alphabet Σ.
	 *
	 * Exemple:
	 * ```
	 * Sigma := "ab"
	 * dico['a' - ASCII_FIRST] =  0
	 * dico['b' - ASCII_FIRST] =  1
	 * dico['c' - ASCII_FIRST] = -1
	 * ```
	 */
	int dico[MAX_SYMBOLES];
};

typedef struct AFN* AFN;


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
AFN afn_init(int Q, int nbInitiaux, const int *listInitiaux, int nbFinals, const int *listFinals, const char *Sigma);


/**
 * Modifie la fonction de transition de l'AFN spécifié de façon à ce que Δ(q1, s) contienne l'état q2.
 */
void afn_ajouter_transition(AFN A, int q1, char s, int q2);


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
AFN afn_finit(const char *filename);


/**
 * Calcul et renvoie l'epsilon-fermeture d'un ensemble d'états `R` trié par ordre croissant et dont le dernier élément `INVALID_STATE`.
 */
int* afn_epsilon_fermeture(AFN A, const int *R);


/**
 * Renvoie `1` si la chaîne spécifiée est acceptée par l'AFN spécifié, sinon renvoie `0`.
 */
int afn_simuler(AFN A, const char *s);


/**
 * Construit et renvoie un AFN acceptant le langage constitué du seul symbole `c`.
 */
AFN afn_char(char c, const char *Sigma);


/**
 * Construit et renvoie l'union de deux AFN.
 */
AFN afn_union(AFN A, AFN B);


/**
 * Affiche l'AFN spécifié dans le flux de sortie standard.
 */
void afn_print(AFN A);


/**
 * Dessine un AFN dans un fichier `filename.png`.
 */
void afn_dot(AFN A, const char *filename);


/**
 * Libère les ressources allouées à un AFN.
 */
void afn_free(AFN A);

#endif // AFN_H
