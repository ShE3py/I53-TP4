#ifndef STACK_H
#define STACK_H

#include <stddef.h>

/**
 * Représente une pile de nombres entiers stockés dans un tableau.
 */
typedef struct {
	/**
	 * Le tableau contenant tous les éléments de la pile.
	 */
	int *buf;
	
	/**
	 * La taille de la pile.
	 */
	size_t len;
	
	/**
	 * La taille du tableau ({@code capacity <= len})
	 */
	size_t capacity;
} stack;

/**
 * Créer une nouvelle pile vide.
 */
stack stack_init();

/**
 * Créer une nouvelle pile à un seul élément.
 */
stack stack_init_one(int val);

/**
 * Créer une nouvelle pile à partir de plusieurs éléments.
 */
stack stack_init_from(const int *values, size_t n);

/**
 * Ajoute un élément à la pile.
 */
void stack_push(stack *s, int val);

/**
 * Ajoute plusieurs éléments à la pile.
 */
void stack_push_all(stack *s, const int *values, size_t n);

/**
 * Réserve de la place pour au moins {@code n} nouveaux éléments.
 */
void stack_reserve(stack *s, size_t n);

/**
 * Dépiler le dernier élément ajouté à la pile. <br />
 * Termine le programme si jamais la pile est vide.
 */
int stack_pop(stack *s);

/**
 * Tente de dépiler le dernier élément ajouté à la pile dans {@code out};
 * renvoie {@code 1} si la pile n'est pas vide, ou {@code 0} si la pile est vide.
 */
int stack_try_pop(stack *s, int *out);

/**
 * Vide la pile sans faire aucune désallocation.
 */
void stack_pop_all(stack *s);

/**
 * Renvoie sans dépiler le dernier élément ajouté à la pile. <br />
 * Termine le programme si jamais la pile est vide.
 */
int stack_peek(stack s);

/**
 * Tente de mettre une copie du dernier élément ajouté à la pile dans {@code out};
 * renvoie {@code 1} si la pile n'est pas vide, ou {@code 0} le cas échéant.
 */
int stack_try_peek(stack s, int *out);

/**
 * Renvoie {@code 1} si jamais la pile est vide, sinon renvoie {@code 0}.
 */
int stack_is_empty(stack s);

/**
 * Désalloue tous les éléments de la pile.
 */
void stack_free(stack *s);

void stack_print(stack s);

#endif // STACK_H
