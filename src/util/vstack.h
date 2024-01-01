#ifndef VSTACK_H
#define VSTACK_H

#include <stddef.h>

#include "afn.h"

/*
 * Copié-collé de stack.h, suppression de certaines fonctions
 * vstack pour value stack, utilisé dans l'analyse syntaxique pour mémoriser les opérandes
 */

/**
 * Représente une pile d'AFN stockés dans un tableau.
 */
typedef struct {
	/**
	 * Le tableau contenant tous les éléments de la pile.
	 */
	AFN *buf;
	
	/**
	 * La taille de la pile.
	 */
	size_t len;
	
	/**
	 * La taille du tableau ({@code capacity <= len})
	 */
	size_t capacity;
} vstack;

/**
 * Créer une nouvelle pile vide avec une capacité initiale de deux éléments.
 */
vstack vstack_new();

/**
 * Ajoute un élément à la pile.
 */
void vstack_push(vstack *s, AFN val);

/**
 * Réserve de la place pour au moins `n` nouveaux éléments.
 */
void vstack_reserve(vstack *s, size_t n);

/**
 * Dépile le dernier élément ajouté à la pile.
 * Termine le programme si jamais la pile est vide.
 */
AFN vstack_pop(vstack *s);

/**
 * Désalloue tous les éléments de la pile.
 */
void vstack_free(vstack *s);

#endif // VSTACK_H
