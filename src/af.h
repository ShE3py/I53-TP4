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

#endif // AF_H
