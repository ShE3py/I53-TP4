#include "util/vstack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/misc.h"

/*
 * Copié-collé de stack.c, suppression de certaines fonctions
 * vstack pour value stack, utilisé dans l'analyse syntaxique pour mémoriser les opérandes
 */

/**
 * Créer une nouvelle pile vide avec une capacité initiale de deux éléments.
 */
vstack vstack_new() {
	vstack s;
	s.buf = checked_malloc(2 * sizeof(AFN));
	s.len = 0;
	s.capacity = 2;
	
	return s;
}


/**
 * Ajoute un élément à la pile.
 */
void vstack_push(vstack *s, AFN val) {
	if(s->len == s->capacity) {
		vstack_reserve(s, 5);
	}
	
	s->buf[s->len] = val;
	++(s->len);
}


/**
 * Réserve de la place pour au moins `n` nouveaux éléments.
 */
void vstack_reserve(vstack *s, size_t n) {
	if(n > 0) {
		s->capacity += n;
		
		if(s->buf == NULL) {
			s->buf = checked_malloc(s->capacity * sizeof(AFN));
		}
		else {
			s->buf = checked_realloc(s->buf, s->capacity * sizeof(AFN));
		}
	}
}


/**
 * Dépile le dernier élément ajouté à la pile.
 * Termine le programme si jamais la pile est vide.
 */
AFN vstack_pop(vstack *s) {
	if(s->len == 0) {
		fprintf(stderr, "pop() on empty vstack");
		exit(1);
	}
	
	AFN val = s->buf[s->len - 1];
	--(s->len);
	
	return val;
}


/**
 * Désalloue tous les éléments de la pile.
 */
void vstack_free(vstack *s) {
	if(s->len != 0) {
		fprintf(stderr, "warning: vstack_free() on a non-empty stack\n");
	}
	
	free(s->buf);
	s->buf = NULL;
	s->len = 0;
	s->capacity = 0;
}
