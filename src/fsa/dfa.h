#ifndef FSA_DFA_H
#define FSA_DFA_H

#include "fsa/mod.h"

/**
 * Représente un automate fini déterministe (AFD).
 */
struct Dfa {
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
	 * L'alphabet de l'automate. Ne termine pas par le caractère nul {@code '\0'}.
	 */
	char *Sigma;
	
	/**
	 * La taille de l'alphabet de l'automate.
	 */
	int lenSigma;
	
	/**
	 * La fonction de transition de l'automate. <br />
	 * δ(q, τ) = {@code delta[q][symbol_index[τ]]}
	 */
	int **delta;
	
	/**
	 * Table de conversion ASCII <-> indice du symbole dans la fonction de transition.
	 */
	int symbol_index[SYMBOL_COUNT];
};

typedef struct Dfa* Dfa;


/**
 * Construit un nouvel AFD sans aucune transition à partir de sa définition. <br />
 * Les ensembles sont passés par pointeurs car cette fonction les consommes et les définira sur {@code NULL} pour empêcher l'appelant de les réutiliser. <br />
 * SAFETY: {@code F} et {@code Sigma} doivent avoir étés alloués avec {@code malloc}.
 */
Dfa dfa_construct(int Q, int q0, int **F, int lenF, char **Sigma, int lenSigma);


/**
 * Modifie la fonction de transition d'un AFD de façon à ce que δ(q1, c) = q2.
 */
void dfa_add_transition(Dfa A, int q1, char c, int q2);


/**
 * Affiche un AFD dans le flux de sortie standard.
 */
void dfa_print(Dfa A);


/**
 * Libère les ressources allouées à un AFD.
 */
void dfa_free(Dfa *A);

#endif // FSA_DFA_H
