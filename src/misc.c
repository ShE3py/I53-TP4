#include "misc.h"

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
	fprintf(stderr, "check_param(): '%s' n'a pas de valeur valide", pname);
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
