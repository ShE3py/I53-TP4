#ifndef AF_H
#define AF_H

/**
 * Tout symbole dans l'alphabet d'un AF aura un code ASCII supérieur ou égal à cette valeur.
 */
#define ASCII_FIRST 38

/**
 * Tout symbole dans l'alphabet d'un AF aura un code ASCII inférieur ou égal à cette valeur.
 */
#define ASCII_LAST 122

/**
 * Le nombre maximal de symboles que peut posséder l'alphabet d'un AF.
 */
#define MAX_SYMBOLES (ASCII_LAST - ASCII_FIRST + 1)

/**
 * Le nom d'un état qui n'appartiendra jamais à un AF.
 */
#define INVALID_STATE -1


/**
 * Initialise le dictionnaire passé en paramètre de telle manière que pour tout caractère
 * ASCII_FIRST <= c <= ASCII_LAST, `dico[c - ASCII_FIRST]` corresponde à l'indice du caractère `c` dans
 * la chaîne `Sigma`, ou à `-1` si `c` n'est pas présent dans `Sigma`.
 *
 * Vérifie aussi qu'aucun caractère n'est dupliqué dans `Sigma`, et que tout caractère dans `Sigma` appartient à
 * l'intervalle [ASCII_FIRST, ASCII_LAST].
 */
void af_init_dico(int dico[MAX_SYMBOLES], const char *Sigma, int lenSigma);

#endif // AF_H
