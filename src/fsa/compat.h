#ifndef FSA_COMPAT_H
#define FSA_COMPAT_H

#include "fsa/dfa.h"

typedef Dfa AFD;

/*
 * FUNCTION: afd_init
 * ------------------
 * initialise et retourne un AFD dont les états sont numérotés de 0 à `Q`
 * 
 * param: 
 *        Q  - plus grand état de l'automate
 *        q0 - état inital    
 *        nbFinals - nombre d'états finals
 *        listFinals - tableau de `nbFinals` entiers représentant les états finals
 *        Sigma - alphabet de l'automate
 * 
 * return:
 *        un AFD dont la tableau de transition est allouée mais vide
 */
AFD afd_init(int Q, int q0, int nbFinals, int *listFinals, char *Sigma);

/*
 * FUNCTION: afd_ajouter_transition
 * --------------------------------
 * ajoute la transition  `q1` -- `s` --> `q2` à l'automate `A`
 * 
 * param: 
 *        A  - un AFD
 *        q1 - état de départ de la transition    
 *        s  - étiquette de la transition
 *        q2 - état d'arrivée de la transition    
 */
void afd_ajouter_transition(AFD A, int q1, char s, int q2);

/*
 * FUNCTION: afd_print
 * -------------------
 * affiche l'AFD `A`
 * 
 * param: 
 *        A  - un AFD
 */
void afd_print(AFD A);

/*
 * FUNCTION: afd_free
 * -------------------
 * libère la mémoire de l'AFD `A` initialisé par la fonction afd_init 
 * 
 * param: 
 *        A  - un AFD
 */
void afd_free(AFD A);

#endif // FSA_COMPAT_H
