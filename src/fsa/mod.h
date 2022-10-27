#ifndef FSA_MOD_H
#define FSA_MOD_H

/**
 * Le code ASCII du premier symbole utilisable du plus grand alphabet supporté (inclusif).
 */
#define FIRST_SYMBOL 38

/**
 * Le code ASCII du dernier symbole utilisable du plus grand alphabet supporté (inclusif).
 */
#define LAST_SYMBOL 122

/**
 * Le nombre de symboles utilisables dans le plus grand alphabet supporté.
 */
#define SYMBOL_COUNT (LAST_SYMBOL - FIRST_SYMBOL + 1)

/**
 * Le seul mot de longueur zéro.
 */
#define EPSILON '&'

/**
 * Représente un état invalide.
 */
#define INVALID_STATE -1

#endif // FSA_MOD_H
