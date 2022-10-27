#include "set.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"

set set_init() {
	set s;
	s.buf = NULL;
	s.len = 0;
	s.capacity = 0;
	
	return s;
}

set set_init_singleton(int val) {
	set s;
	s.buf = checked_malloc(1 * sizeof(int));
	s.buf[0] = val;
	s.len = 1;
	s.capacity = 1;
	
	return s;
}

set set_init_from(const int *values, size_t n) {
	if(n == 0) {
		return set_init();
	}
	
	set s = set_init_singleton(values[0]);
	set_reserve(&s, n - 1);
	
	for(size_t i = 1; i < n; ++i) {
		set_push(&s, values[i]);
	}
	
	return s;
}

int set_push(set *s, int val) {
	// binary search
	size_t i = 0;
	size_t j = s->len;
	
	size_t d;
	while(i < j && (d = (j - i)) != 0) {
		size_t k = i + (d / 2);
		int e = s->buf[k];
		
		if(e < val) {
			i = k + 1;
			
			if(i == s->len) { break; }
		}
		else if(e > val) {
			j = k;
			
			if(j == s->len) { break; }
		}
		else {
			return 0; // already present
		}
	}
	
	// i is now the insertion index of 'val' which maintains sorted order
	
	if(s->len == s->capacity) {
		set_reserve(s, 5);
	}
	
	if(s->len > 0 && i != s->len) {
		j = s->len;
		do {
			--j;
			s->buf[j + 1] = s->buf[j];
		}
		while(j != i);
	}
	
	s->buf[i] = val;
	++(s->len);
	return 1;
}

set set_union(set rhs, set lhs) {
	size_t len = rhs.len + lhs.len;
	if(len == 0) {
		return set_init();
	}
	
	size_t capacity = len;
	int *buf = checked_malloc(capacity * sizeof(int));
	
	size_t i = 0;
	size_t j = 0;
	size_t k = 0;
	
	if(rhs.len != 0 && lhs.len != 0) {
		while(1) {
			if(rhs.buf[i] < lhs.buf[j]) {
				buf[k] = rhs.buf[i];
				++k;
				
				if(++i == rhs.len) {
					break;
				}
			}
			else {
				buf[k] = lhs.buf[j];
				++k;
				
				if(rhs.buf[i] == lhs.buf[j++] && (--len, ++i) == rhs.len) {
					break;
				}
				
				if(j == lhs.len) {
					break;
				}
			}
		}
	}
	
	while(i < rhs.len) {
		buf[k] = rhs.buf[i];
		++i;
		++k;
	}
	
	while(j < lhs.len) {
		buf[k] = lhs.buf[j];
		++j;
		++k;
	}
	
	set s;
	s.buf = buf;
	s.len = len;
	s.capacity = capacity;
	
	return s;
}

int set_are_disjoints(set rhs, set lhs) {
	if(rhs.len == 0 || lhs.len == 0) {
		return 1;
	}
	
	size_t i = 0;
	size_t j = 0;
	
	while(1) {
		while(lhs.buf[j] < rhs.buf[i]) {
			if(++j == lhs.len) {
				// on a atteint la fin de l'ensemble de droite sans avoir rencontré de doublon
				return 1;
			}
		}
		
		//
		// à ce stade, si j > 0, on a
		// lhs.buf[j - 1] < rhs.buf[i]
		//
		// or les ensembles sont triés dans l'odre croissant, on a donc
		// lhs.buf[k] < rhs.buf[i] pour tout 0 <= k < j
		//
		// or cela reste vrai pour rhs, on a donc
		// lhs.buf[k] < rhs.buf[i + m] pour tout 0 <= k < j et 0 <= m < rhs.len
		//
		// il suffit donc d'incrémenter i jusqu'à ce qu'on arrive à la fin d'un ensemble,
		// ou jusqu'à ce que deux éléments soient égaux
		//
		
		if(lhs.buf[j] == rhs.buf[i]) {
			return 0;
		}
		
		if(++i == rhs.len) {
			return 1;
		}
	}
}

void set_reserve(set *s, size_t n) {
	if(n > 0) {
		s->capacity += n;
		
		if(s->buf == NULL) {
			s->buf = checked_malloc(s->capacity * sizeof(int));
		}
		else {
			s->buf = checked_realloc(s->buf, s->capacity * sizeof(int));
		}
	}
}

void set_clear(set *s) {
	s->len = 0;
}

void set_print(set s) {
	if(s.len == 0) {
		printf("{}\n");
	}
	else {
		printf("{");
		
		for(size_t i = 0; i < (s.len - 1); ++i) {
			printf("%i, ", s.buf[i]);
		}
		
		printf("%i}\n", s.buf[s.len - 1]);
	}
}

void set_free(set *s) {
	free(s->buf);
	s->buf = NULL;
	s->len = 0;
	s->capacity = 0;
}
