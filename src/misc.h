#ifndef MISC_H
#define MISC_H

#include <stddef.h>

/**
 * Renvoie {@code malloc(size)}, terminant le programme si celui-ci échoue.
 */
void* checked_malloc(size_t size);

/**
 * Termine le programme en affichant un message d'erreur comme quoi le paramètre {@code pname} n'est pas valide
 * si {@code ok != 1}. 
 */
void check_param(const char *pname, int ok);

/**
 * Concatène deux chaînes de caractères dans une nouvelle chaînée allouée avec {@code malloc}.
 */
char* concat(const char *lhs, const char *rhs);

#endif // MISC_H
