#include "util/misc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__has_attribute) && __has_attribute(noinline)
#	define NO_INLINE __attribute__((noinline))
#else
#	define NO_INLINE
#endif

#if defined(__has_attribute) && __has_attribute(cold)
#	define COLD __attribute__((cold))
#else
#	define COLD
#endif

COLD NO_INLINE
static void bad_alloc() {
	fprintf(stderr, "échec d'allocation dynamique\n");
	exit(1);
}

COLD NO_INLINE
static void bad_param(const char *pname) {
	fprintf(stderr, "check_param(): '%s' n'a pas de valeur valide\n", pname);
	exit(1);
}


/**
 * Renvoie `malloc(size)`, terminant le programme si l'allocation échoue.
 */
void* checked_malloc(size_t size) {
	check_param("size", size != 0);
	
	void* ptr = malloc(size);
	if(!ptr) {
		bad_alloc();
	}
	
	return ptr;
}


/**
 * Renvoie `realloc(ptr, new_size)`, terminant le programme si la réallocation échoue.
 */
void* checked_realloc(void *ptr, size_t new_size) {
	check_param("new_size", new_size != 0);
	
	void* new_ptr = realloc(ptr, new_size);
	if(!new_ptr) {
		free(ptr);
		bad_alloc();
	}
	
	return new_ptr;
}


/**
 * Si `!ok`, termine le programme en affichant un message d'erreur comme quoi le paramètre `pname` n'est pas valide.
 */
void check_param(const char *pname, int ok) {
	if(ok != 1) {
		bad_param(pname);
	}
}


/**
 * Concatène deux chaînes de caractères dans une nouvelle chaînée que l'appelant devra `free()`.
 */
char* concat(const char *lhs, const char *rhs) {
	size_t lhsl = strlen(lhs);
	size_t rhsl = strlen(rhs);
	
	char *out = malloc(lhsl + rhsl + 1);
	memcpy(out, lhs, lhsl);
	memcpy(out + lhsl, rhs, rhsl + 1);
	
	return out;
}


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
int fparse_int_base(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, char **bufPtr, size_t *bufLen, int stricly_positive) {
	// vérification des paramètres
	check_param("f", f != NULL);
	check_param("fline", fline != NULL);
	check_param("buf", buf != NULL);
	check_param("bufCapacity", bufCapacity != NULL);
	
	char *bufEnd = (*buf != NULL) ? (*buf + *bufLen) : NULL;
	
	check_param("bufPtr", bufPtr != NULL && (*bufPtr == NULL || (*bufPtr >= *buf && *bufPtr < bufEnd)));
	check_param("bufLen", bufLen != NULL);
	
	// est-ce qu'on bien a une ligne à lire ?
	int ok;
	
	if(*bufPtr != NULL) {
		// *bufPtr != NULL implique que *buf contient une ligne *bufPtr...bufEnd qu'on n'a pas encore fini de lire
		ok = 1;
	}
	else {
		// *bufPtr == NULL implique qu'il faut lire la ligne suivante car l'éventuelle ligne précédente a déjà été lue entièrement
		++(*fline);
		*bufLen = getline(buf, bufCapacity, f);
		
		if(*bufLen == -1) {
			*bufLen = 0;
			ok = 0;
		}
		else {
			*bufPtr = *buf;
			ok = 1;
		}
	}
	
	if(ok) {
		if(*bufLen == 0 || !isdigit(**bufPtr)) {
			// le premier caractère doit être un chiffre
			fprintf(stderr, "%s:%li:%li: nombre entier malformé\n", fpath, *fline, *bufPtr - *buf);
			exit(1);
		}
		
		// le premier caractère est un chiffre, on lit le nombre jusqu'à la fin de la ligne ou qu'on rencontre un non-chiffre
		
		int val = 0;
		do {
			val = val * 10 + (**bufPtr - '0');
		}
		while(++(*bufPtr) != bufEnd && isdigit(**bufPtr));
		
		
		if(*bufPtr != bufEnd) {
			// si on a quitté la boucle à cause de la rencontre d'un non-chiffre et que ce non-chiffre n'est pas une espace, erreur
			if(!isspace(**bufPtr)) {
				fprintf(stderr, "%s:%li:%li: nombre entier malformé: caractère inattendu '%c'\n", fpath, *fline,  *bufPtr - *buf, **bufPtr);
				exit(1);
			}
			
			// sinon, on déplace le début de la prochaine lecture après cette espace
			++(*bufPtr);
		}
		
		
		if(*bufPtr == bufEnd || **bufPtr == '\0') {
			// on quitte la ligne actuelle si on est arrivé au bout de celle-ci
			*bufPtr = NULL;
		}
		
		return val;
	}
	else {
		fprintf(stderr, "%s: nombre entier attendu, fin du fichier\n", fpath);
		exit(1);
	}
}


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
int fparse_int(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int stricly_positive) {
	char *bufPtr = NULL;
	size_t bufLen = 0;
	
	int val = fparse_int_base(f, fpath, fline, buf, bufCapacity, &bufPtr, &bufLen, stricly_positive);
	if(bufPtr != NULL) {
		fprintf(stderr, "%s:%li:%li: nombre entier malformé: caractères en surplus\n", fpath, *fline, bufPtr - *buf);
		exit(1);
	}
	
	return val;
}


/**
 * Converti les deux prochaines lignes du flux entrant spécifié en un ensemble de nombres entiers positifs.
 *
  * Paramètres:
 * - f          : le flux entrant
 * - fpath      : le nom du flux entrant (utilisé pour les messages d'erreur)
 * - fline      : un pointeur vers le numéro de la ligne dans le flux sortant (sera incrémenté automatiquement, utilisé pour les messages d'erreur)
 * - buf        : un pointeur vers une zone tampon qui sera réalloué au besoin (`*buf` peut être `NULL`)
 * - bufCapacity: un pointeur vers la taille de la zone tampon (sera augmenté au besoin)
 * - outLen     : un pointeur qui sera modifié pour contenir la taille finale de l'ensemble lu
 * - Q          : la valeur maximale (incl.) des éléments de cet ensemble
 *
 * Voir aussi:
 * - `fparse_int(FILE*, const char*, size_t*, char**, size_t*, int)`
 * - `fparse_int_base(FILE*, const char*, size_t*, char**, size_t*, char**, size_t*, int)`
 */
int* fparse_int_set(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *outLen, int Q) {
	// la première ligne contient la taille de l'ensemble
	int len = fparse_int(f, fpath, fline, buf, bufCapacity, 1);
	int *set = malloc(len * sizeof(int));
	
	// la deuxième ligne contient les éléments de l'ensemble
	char *bufPtr = NULL;
	size_t bufLen = 0;
	for(int i = 0; i < len; ++i) {
		set[i] = fparse_int_base(f, fpath, fline, buf, bufCapacity, &bufPtr, &bufLen, 0);
		
		if(set[i] > Q) {
			fprintf(stderr, "%s:%li:%li: q > Q", fpath, *fline, bufPtr - *buf - 1);
			exit(1);
		}
	}
	
	if(bufPtr != NULL) {
		fprintf(stderr, "%s:%li:%li: l'ensemble est plus grand que la taille spécifiée par la ligne précédente\n", fpath, *fline, bufPtr - *buf);
	}
	
	*outLen = len;
	return set;
}


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
char* fparse_Sigma(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *outLen) {
	int len;
	
	if((len = getline(buf, bufCapacity, f)) > 0) {
		char *line = *buf;
		int last = len - 1;
		
		if(line[last] == '\n') {
			line[last] = '\0';
		}
		
		char *Sigma = checked_malloc(len);
		memcpy(Sigma, line, len);
		
		*outLen = len - 1;
		return Sigma;
	}
	else {
		fprintf(stderr, "%s:%li:1: fin du fichier, alphabet attendu\n", fpath, *fline);
		exit(1);
	}
}


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
int fparse_transition(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *q1, char *c, int *q2) {
	ssize_t read;
	
	if((read = getline(buf, bufCapacity, f)) > 0) {
		size_t bufLen = read;
		char *bufPtr = *buf;
		
		*q1 = fparse_int_base(f, fpath, fline, buf, bufCapacity, &bufPtr, &bufLen, 0);
		
		if(bufPtr == NULL) {
			fprintf(stderr, "%s:%li:%li: transition malformée: symbole attendu\n", fpath, *fline, bufPtr - *buf);
			exit(1);
		}
		
		*c = *bufPtr;
		
		char *bufEnd = *buf + bufLen;
		++bufPtr;
		if(bufPtr == bufEnd || !isspace(*bufPtr) || *bufPtr == '\0' || ++bufPtr == bufEnd) {
			fprintf(stderr, "%s:%li:%li: transition malformée: un second état est attendu", fpath, *fline, bufPtr - *buf);
			exit(1);
		}
		
		*q2 = fparse_int_base(f, fpath, fline, buf, bufCapacity, &bufPtr, &bufLen, 0);
		
		// af[dn]_ajouter_transition() vérifiera les valeurs
		return 1;
	}
	else {
		return 0;
	}
}
