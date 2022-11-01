#ifndef AFD_H
#define AFD_H

#include "af.h"

/**
 * Représente un automate fini déterministe (AFD).
 */
struct AFD {
	/**
	 * Le plus grand état de l'automate.
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
	 */
	int **delta;
	
	/**
	 * Table de conversion code ASCII de τ <-> indice du symbole τ dans la fonction de transition.
	 */
	int dico[MAX_SYMBOLES];
};

typedef struct AFD* AFD;


/**
 * Initialise et renvoie un nouvel AFD à partir de sa définition.
 */
AFD afd_init(int Q, int q0, int nbFinals, int *listFinals, char *Sigma);


/**
 * Modifie la fonction de transition de l'AFD spécifié de façon à ce que δ(q1, s) = q2.
 */
void afd_ajouter_transition(AFD A, int q1, char s, int q2);


/**
 * Initialise et renvoie un nouvel AFD à partir d'un fichier `filename` écrit au format :
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
