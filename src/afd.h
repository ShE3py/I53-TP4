#ifndef AFD_H
#define AFD_H

#include "af.h"

/**
 * Représente un automate fini déterministe (AFD).
 *
 * Les états sont représentés par des nombres entiers positifs.
 */
struct AFD {
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
	 * L'état initial de l'automate.
	 */
	int q0;
	
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
	 * δ(q, τ) = delta[q][dico[τ - ASCII_FIRST]]
	 *
	 * La fonction est définie pout tout état q et pour tout symbole τ dans Σ.
	 */
	int **delta;
	
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

typedef struct AFD* AFD;


/**
 * Initialise et renvoie un nouvel AFD à partir de sa définition.
 *
 * Paramètres:
 * - Q          : le plus grand état
 * - q0         : l'état initial
 * - nbFinals   : le nombre d'états finaux
 * - listFinals : un tableau des états finaux
 * - Sigma      : une chaîne de caractères terminée par '\0' qui représentera l'alphabet
 *
 * Remarques:
 * - La fonction de transition de l'automate est allouée mais indéfinie dans le nouvel AFD.
 * - Le comportement si un état final est présent deux fois dans `listFinals` est indéfini.
 *
 * Voir aussi:
 * - `afd_ajouter_transition(AFD, int, char, int)`
 */
AFD afd_init(int Q, int q0, int nbFinals, const int *listFinals, const char *Sigma);


/**
 * Modifie la fonction de transition de l'AFD spécifié de façon à ce que δ(q1, s) = q2.
 */
void afd_ajouter_transition(AFD A, int q1, char s, int q2);


/**
 * Initialise et renvoie un nouvel AFD à partir d'un fichier écrit au format suivant :
 * ```
 * Q
 * q0
 * lenF
 * F[0] F[1] ... F[lenF - 1]
 * Sigma
 * q0 τ0 q'0
 * q1 τ1 q'1
 * ...
 * qk τk q'k
 * ```
 */
AFD afd_finit(char *filename);


/**
 * Renvoie `1` si la chaîne spécifiée est acceptée par l'AFD spécifié, sinon renvoie `0`.
 */
int afd_simuler(AFD A, const char *s);


/**
 * Affiche l'AFD spécifié dans le flux de sortie standard.
 */
void afd_print(AFD A);


/**
 * Libère les ressources allouées à un AFD.
 */
void afd_free(AFD A);

#endif // AFD_H
