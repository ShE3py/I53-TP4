#ifndef SET_H
#define SET_H

#include <stddef.h>

/**
 * Représente un ensemble ordonné d'entiers.
 */
typedef struct {
	/**
	 * Le tableau contenant tous les éléments de l'ensemble.
	 */
	int *buf;
	
	/**
	 * La taille de l'ensemble.
	 */
	size_t len;
	
	/**
	 * La taille de l'ensemble ({@code capacity <= len})
	 */
	size_t capacity;
} set;

/**
 * Créer un nouvel ensemble vide.
 */
set set_new_empty();

/**
 * Créer un nouvel ensemble avec un seul élément.
 */
set set_new_singleton(int val);

/**
 * Créer un nouvel ensemble à partir de plusieurs éléments.
 */
set set_copy_from(const int *values, size_t n);

/**
 * Rajoute un nouvel élément à l'ensemble, renvoyant `0` si l'élément est déjà présent.
 */
int set_push(set *s, int val);

/**
 * Calcul l'union de deux ensembles.
 */
set set_union(set rhs, set lhs);

/**
 * Renvoie `1` si les deux ensembles sont disjoints, sinon renvoie `0`.
 */
int set_are_disjoints(set rhs, set lhs);

/**
 * Renvoie `1` si les deux ensembles se croisent, sinon renvoie `0`.
 */
int set_are_intersecting(set rhs, set lhs);

/**
 * Réserve de la place pour au moins `n` nouveaux éléments.
 */
void set_reserve(set *s, size_t n);

/**
 * Retire tous les éléments de l'ensemble sans libéré d'espace mémoire.
 */
void set_clear(set *s);

/**
 * Affiche l'ensemble dans la sortie standard.
 */
void set_print(set s);

/**
 * Libère l'espace mémoire occupé par cet ensemble.
 */
void set_free(set *s);

#endif // SET_H
