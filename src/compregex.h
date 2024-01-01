#ifndef COMPREGEX_H
#define COMPREGEX_H

#include "afn.h"

/**
 * Transforme la chaîne de caractères spécifiée en un AFN.
 */
AFN compile(const char *s);

#endif // COMPREGEX_H
