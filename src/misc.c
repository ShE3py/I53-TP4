#include "misc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __has_attribute
#	if __has_attribute(noinline)
#		define INLINE_NEVER __attribute__((noinline))
#	endif

#	if __has_attribute(cold)
#		define COLD __attribute__((cold))
#	endif
#endif

#ifndef INLINE_NEVER
#	define INLINE_NEVER
#endif

#ifndef COLD
#	define COLD
#endif

COLD INLINE_NEVER
void bad_malloc() {
	fprintf(stderr, "malloc(): échec d'allocation");
	exit(1);
}

COLD INLINE_NEVER
void bad_param(const char *pname) {
	fprintf(stderr, "check_param(): '%s' n'a pas de valeur valide\n", pname);
	exit(1);
}

void* checked_malloc(size_t size) {
	check_param("size", size != 0);
	
	void* ptr = malloc(size);
	if(!ptr) {
		bad_malloc();
	}
	
	return ptr;
}

void check_param(const char *pname, int ok) {
	if(ok != 1) {
		bad_param(pname);
	}
}

char* concat(const char *lhs, const char *rhs) {
	size_t lhsl = strlen(lhs);
	size_t rhsl = strlen(rhs);
	
	char *out = malloc(lhsl + rhsl + 1);
	memcpy(out, lhs, lhsl);
	memcpy(out + lhsl, rhs, rhsl + 1);
	
	return out;
}

int fparse_int_base(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, char **bufPtr, size_t *bufLen, int stricly_positive) {
	check_param("f", f != NULL);
	check_param("fline", fline != NULL);
	check_param("buf", buf != NULL);
	check_param("bufCapacity", bufCapacity != NULL);
	
	char *bufEnd = *buf + *bufLen;
	check_param("bufPtr", bufPtr != NULL && (*bufPtr == NULL || (*bufPtr >= *buf && *bufPtr < bufEnd)));
	check_param("bufLen", bufLen != NULL);
	
	if(*bufPtr != NULL || (++(*fline), *bufLen = getline(buf, bufCapacity, f), *bufPtr = *buf, *bufLen) != -1) {
		if(*bufLen == 0 || !isdigit(**bufPtr)) {
			fprintf(stderr, "%s:%li:%li: nombre entier malformé\n", fpath, *fline,  *bufPtr - *buf);
			exit(1);
		}
		
		int val = 0;
		do {
			val = val * 10 + (**bufPtr - '0');
		}
		while(++(*bufPtr) != bufEnd && isdigit(**bufPtr));
		
		if(*bufPtr != bufEnd) {
			if(!isspace(**bufPtr)) {
				fprintf(stderr, "%s:%li:%li: nombre entier malformé: caractère inattendu '%c'\n", fpath, *fline,  *bufPtr - *buf, **bufPtr);
				exit(1);
			}
			
			++(*bufPtr);
		}
		
		if(*bufPtr == bufEnd || **bufPtr == '\0') {
			*bufPtr = NULL;
		}
		
		return val;
	}
	else {
		fprintf(stderr, "%s: nombre entier attendu, fin du fichier\n", fpath);
		exit(1);
	}
}

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

int* fparse_int_set(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *outLen, int Q) {
	int len = fparse_int(f, fpath, fline, buf, bufCapacity, 1);
	int *set = malloc(len * sizeof(int));
	
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

int fparse_transition(FILE *f, const char *fpath, size_t *fline, char **buf, size_t *bufCapacity, int *q1, char *c, int *q2) {
	size_t bufLen;
	
	if((bufLen = getline(buf, bufCapacity, f)) != -1) {
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
		
		// dfa_add_transition() vérifiera les valeurs
		return 1;
	}
	else {
		return 0;
	}
}
