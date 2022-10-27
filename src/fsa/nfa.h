#ifndef FSA_NFA_H
#define FSA_NFA_H

#include "fsa/mod.h"
#include "set.h"

/**
 * Représente un automate fini non-déterministe (AFN).
 */
struct Nfa {
	/**
	 * Le plus grand état de l'automate.
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
	 * L'alphabet de l'automate. Ne termine pas par le caractère nul {@code '\0'}.
	 */
	char *Sigma;
	
	/**
	 * La taille de l'alphabet de l'automate.
	 */
	int lenSigma;
	
	/**
	 * La fonction de transition de l'automate. <br />
	 * Δ(q, τ) = {@code Delta[q][symbol_index[τ]]} <br />
	 * La fin d'un ensemble dans l'ensemble des parties est marquée par {@code INVALID_STATE}. <br />
	 * L'ensemble vide peut être noté par {@code \{INVALID_STATE\}} ou {@code NULL}.
	 */
	int ***Delta;
	
	/**
	 * Table de conversion ASCII <-> indice du symbole dans la fonction de transition.
	 */
	int symbol_index[SYMBOL_COUNT];
};

typedef struct Nfa* Nfa;

/**
 * Construit un nouvel AFN sans aucune transition à partir de sa définition. <br />
 * Les ensembles sont passés par pointeurs car cette fonction les consommes et les définira sur {@code NULL} pour empêcher l'appelant de les réutiliser. <br />
 * SAFETY: {@code I}, {@code F} et {@code Sigma} doivent avoir étés alloués avec {@code malloc}.
 */
Nfa nfa_construct(int Q, int **I, int lenI, int **F, int lenF, char **Sigma, int lenSigma);

/**
 * Converti le fichier spécifié en un nouvel AFN.
 */
Nfa nfa_parse(char *path);

/**
 * Modifie la fonction de transition d'un AFN de façon à ce que q2 appartiennent à Δ(q1, c).
 */
void nfa_add_transition(Nfa A, int q1, char c, int q2);

void nfa_epsilon_closure_in_place(Nfa A, set *G);

/**
 * Renvoie {@code 1} si jamais la chaîne spécifiée est accepté par l'AFN spécifié.
 */
int nfa_is_accepted(Nfa A, const char *s);

/**
 * Affiche un AFN dans le flux de sortie standard.
 */
void nfa_print(Nfa A);

/**
 * Dessine un AFN dans le fichier spécifié.
 */
void nfa_dot(Nfa A, const char *path);

/**
 * Libère les ressources allouées à un AFD.
 */
void nfa_free(Nfa *A);

#endif // FSA_NFA_H
