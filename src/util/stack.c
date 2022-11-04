#include "util/stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/misc.h"

/**
 * Créer une nouvelle pile vide.
 */
stack stack_new_empty() {
	stack s;
	s.buf = NULL;
	s.len = 0;
	s.capacity = 0;
	
	return s;
}


/**
 * Créer une nouvelle pile à un seul élément.
 */
stack stack_new_with_value(int val) {
	stack s;
	s.buf = checked_malloc(1 * sizeof(int));
	s.buf[0] = val;
	s.len = 1;
	s.capacity = 1;
	
	return s;
}


/**
 * Créer une nouvelle pile à partir de plusieurs éléments.
 */
stack stack_copy_from(const int *values, size_t n) {
	if(n == 0) {
		return stack_new_empty();
	}
	
	stack s;
	s.buf = checked_malloc(n * sizeof(int));
	memcpy(s.buf, values, n * sizeof(int));
	s.len = n;
	s.capacity = n;
	
	return s;
}


/**
 * Ajoute un élément à la pile.
 */
void stack_push(stack *s, int val) {
	if(s->len == s->capacity) {
		stack_reserve(s, 5);
	}
	
	s->buf[s->len] = val;
	++(s->len);
}


/**
 * Ajoute plusieurs éléments à la pile.
 */
void stack_push_all(stack *s, const int *values, size_t n) {
	if((s->len + n) > s->capacity) {
		stack_reserve(s, (s->len + n) - s->capacity);
	}
	
	memcpy(s->buf + s->len, values, n * sizeof(int));
	s->len += n;
}


/**
 * Réserve de la place pour au moins `n` nouveaux éléments.
 */
void stack_reserve(stack *s, size_t n) {
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


/**
 * Dépile le dernier élément ajouté à la pile.
 * Termine le programme si jamais la pile est vide.
 */
int stack_pop(stack *s) {
	int val = stack_peek(*s);
	--(s->len);
	
	return val;
}


/**
 * Tente de dépiler le dernier élément ajouté à la pile dans `out`;
 * renvoie `1` si la pile n'est pas vide, ou `0` si la pile est vide.
 */
int stack_try_pop(stack *s, int *out) {
	if(stack_try_peek(*s, out)) {
		--(s->len);
		return 1;
	}
	
	return 0;
}


/**
 * Vide la pile sans faire aucune désallocation.
 */
void stack_pop_all(stack *s) {
	s->len = 0;
}


/**
 * Renvoie sans dépiler le dernier élément ajouté à la pile.
 * Termine le programme si jamais la pile est vide.
 */
int stack_peek(stack s) {
	if(stack_is_empty(s)) {
		fprintf(stderr, "peek() on empty stack");
		exit(1);
	}
	
	return s.buf[s.len - 1];
}


/**
 * Tente de mettre une copie du dernier élément ajouté à la pile dans `out`;
 * renvoie `1` si la pile n'est pas vide, ou `0` le cas échéant.
 */
int stack_try_peek(stack s, int *out) {
	if(stack_is_empty(s)) {
		return 0;
	}
	
	*out = s.buf[s.len - 1];
	return 1;
}


/**
 * Renvoie `1` si jamais la pile est vide, sinon renvoie `0`.
 */
int stack_is_empty(stack s) {
	return s.len == 0;
}


/**
 * Affiche la pile dans la sortie standard.
 */
void stack_print(stack s) {
	if(s.len == 0) {
		printf("[]\n");
	}
	else {
		printf("[");
		
		for(size_t i = 0; i < (s.len - 1); ++i) {
			printf("%i, ", s.buf[i]);
		}
		
		printf("%i]\n", s.buf[s.len - 1]);
	}
}


/**
 * Désalloue tous les éléments de la pile.
 */
void stack_free(stack *s) {
	free(s->buf);
	s->buf = NULL;
	s->len = 0;
	s->capacity = 0;
}
