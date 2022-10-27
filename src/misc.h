#ifndef MISC_H
#define MISC_H

#include <stdio.h>

/**
 * Renvoie {@code malloc(size)}, terminant le programme si celui-ci échoue.
 */
void* checked_malloc(size_t size);

/**
 * Renvoie {@code realloc(ptr, new_size)}, terminant le programme si celui-ci échoue.
 */
void* checked_realloc(void *ptr, size_t new_size);

/**
 * Termine le programme en affichant un message d'erreur comme quoi le paramètre {@code pname} n'est pas valide
 * si {@code ok != 1}. 
 */
void check_param(const char *pname, int ok);

/**
 * Concatène deux chaînes de caractères dans une nouvelle chaînée allouée avec {@code malloc}.
 */
char* concat(const char *lhs, const char *rhs);

/**
 * Converti les prochaines caractères du flux entrant spécifié en un nombre entier positif ou strictement positif.
 */
int fparse_int_base(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, char **bufPtr, size_t *bufLen, int stricly_positive);

/**
 * Converti la prochaine ligne du flux entrant spécifié en un nombre entier positif ou strictement positif.
 */
int fparse_int(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int stricly_positive);

/**
 * Converti les prochaines lignes du flux entrant spécifié en un ensemble de nombres entiers positifs.
 */
int* fparse_int_set(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *outLen, int Q);

/**
 * Tente de convertir la prochaine ligne du flux entrant spécifié en une transition. <br />
 * Renvoit {@code 0} en cas de fin de fichier.
 */
int fparse_transition(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *q1, char *c, int *q2);

#endif // MISC_H
