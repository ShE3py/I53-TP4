#ifndef MISC_H
#define MISC_H

#include <stdio.h>

/**
 * Renvoie `malloc(size)`, terminant le programme si l'allocation échoue.
 */
void* checked_malloc(size_t size);


/**
 * Renvoie `realloc(ptr, new_size)`, terminant le programme si la réallocation échoue.
 */
void* checked_realloc(void *ptr, size_t new_size);


/**
 * Si `!ok`, termine le programme en affichant un message d'erreur comme quoi le paramètre `pname` n'est pas valide.
 */
void check_param(const char *pname, int ok);


/**
 * Concatène deux chaînes de caractères dans une nouvelle chaînée que l'appelant devra `free()`.
 */
char* concat(const char *lhs, const char *rhs);


/**
 * Convertis les prochains caractères dans une ligne du flux entrant spécifié en un nombre entier positif.
 *
 * Paramètres:
 * - f               : le flux entrant
 * - fpath           : le nom du flux entrant (utilisé pour les messages d'erreur)
 * - fline           : un pointeur vers le numéro de la ligne dans le flux sortant (sera incrémenté automatiquement, utilisé pour les messages d'erreur)
 * - buf             : un pointeur vers une zone tampon qui sera réalloué au besoin (`*buf` peut être `NULL`)
 * - bufCapacity     : un pointeur vers la taille de la zone tampon (sera augmenté au besoin)
 * - bufPtr          : un pointeur vers un élement de la zone tampon qui servira de début de lecture dans la ligne actuelle (`*bufPtr` peut être `NULL`)
 * - bufLen          : un pointeur vers la longueur réelle de la zone tampon (c.-à-d., la longueur de la ligne en cours de lecture)
 * - stricly_positive: un booléen représentant si la valeur zéro doit être refusée.
 *
 * Voir aussi:
 * - `fparse_int(FILE*, const char*, size_t*, char**, size_t*, int)`
 * - `fparse_int_set(FILE*, const char*, size_t*, char**, size_t*, int*, int)`
 */
int fparse_int_base(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, char **bufPtr, size_t *bufLen, int stricly_positive);


/**
 * Convertis la prochaine ligne du flux entrant spécifié en un nombre entier positif.
 *
 * Paramètres:
 * - f               : le flux entrant
 * - fpath           : le nom du flux entrant (utilisé pour les messages d'erreur)
 * - fline           : un pointeur vers le numéro de la ligne dans le flux sortant (sera incrémenté automatiquement, utilisé pour les messages d'erreur)
 * - buf             : un pointeur vers une zone tampon qui sera réalloué au besoin (`*buf` peut être `NULL`)
 * - bufCapacity     : un pointeur vers la taille de la zone tampon (sera augmenté au besoin)
 * - stricly_positive: un booléen représentant si la valeur zéro doit être refusée.
 *
 * Voir aussi:
 * - `fparse_int_set(FILE*, const char*, size_t*, char**, size_t*, int*, int)`
 * - `fparse_int_base(FILE*, const char*, size_t*, char**, size_t*, char**, size_t*, int)`
 */
int fparse_int(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int stricly_positive);


/**
 * Converti les deux prochaines lignes du flux entrant spécifié en un ensemble de nombres entiers positifs.
 *
  * Paramètres:
 * - f          : le flux entrant
 * - fpath      : le nom du flux entrant (utilisé pour les messages d'erreur)
 * - fline      : un pointeur vers le numéro de la ligne dans le flux sortant (sera incrémenté automatiquement, utilisé pour les messages d'erreur)
 * - buf        : un pointeur vers une zone tampon qui sera réalloué au besoin (`*buf` peut être `NULL`)
 * - bufCapacity: un pointeur vers la taille de la zone tampon (sera augmenté au besoin)
 * - outLen     : un pointeur qui sera modifié pour contenir la taille finale de l'ensemble
 * - Q          : la valeur maximale (incl.) des éléments de cet ensemble
 *
 * Voir aussi:
 * - `fparse_int(FILE*, const char*, size_t*, char**, size_t*, int)`
 * - `fparse_int_base(FILE*, const char*, size_t*, char**, size_t*, char**, size_t*, int)`
 */
int* fparse_int_set(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *outLen, int Q);


/**
 * Converti la prochaines ligne du flux entrant spécifié en un alphabet.
 *
  * Paramètres:
 * - f          : le flux entrant
 * - fpath      : le nom du flux entrant (utilisé pour les messages d'erreur)
 * - fline      : un pointeur vers le numéro de la ligne dans le flux sortant (sera incrémenté automatiquement, utilisé pour les messages d'erreur)
 * - buf        : un pointeur vers une zone tampon qui sera réalloué au besoin (`*buf` peut être `NULL`)
 * - bufCapacity: un pointeur vers la taille de la zone tampon (sera augmenté au besoin)
 * - outLen     : un pointeur qui sera modifié pour contenir la taille finale de l'alphabet
 *
 * Voir aussi:
 * - `fparse_int(FILE*, const char*, size_t*, char**, size_t*, int)`
 * - `fparse_int_base(FILE*, const char*, size_t*, char**, size_t*, char**, size_t*, int)`
 */
char* fparse_Sigma(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *outLen);


/**
 * S'il reste une ligne dans le flux entrant spécifié, la converti en une transition et renvoie `1` ; sinon, renvoie `0`.
 *
 * Paramètres:
 * - f          : le flux entrant
 * - fpath      : le nom du flux entrant (utilisé pour les messages d'erreur)
 * - fline      : un pointeur vers le numéro de la ligne dans le flux sortant (sera incrémenté automatiquement, utilisé pour les messages d'erreur)
 * - buf        : un pointeur vers une zone tampon qui sera réalloué au besoin (`*buf` peut être `NULL`)
 * - bufCapacity: un pointeur vers la taille de la zone tampon (sera augmenté au besoin)
 * - q1         : un pointeur qui sera modifié pour contenir l'élément `q1` de la transition lue
 * - c          : un pointeur qui sera modifié pour contenir l'élément `c` de la transition lue
 * - q2         : un pointeur qui sera modifié pour contenir l'élément `q2` de la transition lue
 */
int fparse_transition(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *q1, char *c, int *q2);

#endif // MISC_H
