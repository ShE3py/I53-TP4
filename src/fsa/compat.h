#ifndef FSA_COMPAT_H
#define FSA_COMPAT_H

#include "fsa/dfa.h"
#include "fsa/nfa.h"

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
 * FUNCTION: afd_finit
 * ------------------
 * initialise et renvoie un AFD à partir du fichier `file` écrit au format:
 * 
 *  'nombre_etat
 *  'etat_initial
 *  'nombre_etats_finals
 *  'etat_final_1 etat_final_2 ... etat_final_n
 *  'alphabet
 *  'etat_i1 symbole_k1 etat_j1
 *  'etat_i2 symbole_k2 etat_j2
 *  '...
 *  'etat_in symbole_kn etat_jn 
 * 
 * param :
 *         file - un nom de fichier
 * return:
 *         un AFD complet
 */
AFD afd_finit(char *file);

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
 * FUNCTION: afd_simuler
 * ---------------------
 * renvoie 1 si la chaine `s` est acceptée par l'AFD `A`, 0 sinon
 *
 * param:
 *        A - un AFD
 *        s - la chaine de caractères à analyser
 */
int afd_simuler(AFD A, char *s);

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

typedef Nfa AFN;

/*
 * FUNCTION: afn_init
 * ------------------
 * initialise et retourne un AFN dont les états sont numérotés de 0 à `Q`
 * ajoute le symbole '&' en début d'alphabet si celui-ci n'y est pas déjà
 * 
 * param: 
 *        Q  - plus grand état de l'automate
 *        nbInitaux - nombre d'états initiaux
 *        listInitiaux - tableau de `nbInitiaux` entiers représentant les états initiaux
 *        nbFinals - nombre d'états finals
 *        listFinals - tableau de `nbFinals` entiers représentant les états finals
 *        Sigma - alphabet de l'automate
 * 
 * return:
 *        un AFN dont la tableau de transition est allouée mais vide
 */
AFN afn_init(int Q, int nbInitiaux, int *listInitiaux, int nbFinals, int *listFinals, char *Sigma);

/*
 * FUNCTION: afn_finit
 * ------------------
 * initialise et renvoie un AFN à partir du fichier `file` écrit au format:
 * 
 *  'nombre_etat
 *  'nombre_etats_initiaux
 *  'etat_initial_1 etat_initial_2 ... etat_initial_n
 *  'nombre_etats_finals
 *  'etat_final_1 etat_final_2 ... etat_final_n
 *  'alphabet
 *  'etat_i1 symbole_k1 etat_j1
 *  'etat_i2 symbole_k2 etat_j2
 *  '...
 *  'etat_in symbole_kn etat_jn 
 * 
 * param :
 *         file - un nom de fichier
 * return:
 *         un AFN complet
 */
AFN afn_finit(char *file);

/*
 * FUNCTION: afn_ajouter_transition
 * --------------------------------
 * ajoute la transition  `q1` -- `s` --> `q2` à l'automate `A` où l'ensemble des transitions
 * partant de l'état `q1` et étiquetées par le symbole `s` delta[q][s] est un tableau 
 * d'entiers trié dans l'ordre croissant et se terminant par -1, NULL si vide
 * 
 * param: 
 *        A  - un AFN
 *        q1 - état de départ de la transition    
 *        s  - étiquette de la transition
 *        q2 - état d'arrivée de la transition    
 */
void afn_ajouter_transition(AFN A, int q1, char s, int q2);

/*
 * FUNCTION: afn_print
 * -------------------
 * affiche l'AFN `A`
 * 
 * param: 
 *        A  - un AFN
 */
void afn_print(AFN A);

/*
 * FUNCTION: afn_free
 * -------------------
 * libère la mémoire de l'AFN `A` initialisé par la fonction afn_init 
 * 
 * param: 
 *        A  - un AFN
 */
void afn_free(AFN A);

#endif // FSA_COMPAT_H
