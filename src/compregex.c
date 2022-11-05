#include "compregex.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/vstack.h"
#include "util/misc.h"

/**
 * Représente le nom d'une unité lexicale.
 */
typedef enum {
	Character,
	Operator,
	Parenthesis
} Kind;


/**
 * Représente un symbole lexicale.
 */
typedef struct {
	Kind kind;
	char value;
} Token;


/**
 * Représente une unité lexicale.
 */
typedef struct {
	Token token;
	size_t index;
} Lexeme;


/**
 * Renvoie `1` si `c` correspond à un opérateur, sinon renvoie `0`.
 */
int isop(char c) {
	return c == '+' || c == '.' || c == '*';
}


/**
 * Renvoie `1` si `c` correspond à une parenthèses, sinon renvoie `0`.
 */
int isparenthesis(char c) {
	return c == '(' || c == ')';
}


/**
 * Affiche une ligne suivit de `carrets` `^` pointant sur un caractère de cette ligne dans
 * le flux d'erreur standard.
 */
void eprintln(const char *s, size_t i, size_t carrets) {	
	char *where = checked_malloc(i + carrets + 1);
	
	for(size_t j = 0; j < i; ++j) {
		where[j] = ' ';
	}
	
	for(size_t j = 0; j < carrets; ++j) {
		where[i + j] = '^';
	}
	
	where[i + carrets] = '\0';
	
	fprintf(stderr, "%s\n%s\n\n", s, where);
	free(where);
}


/**
 * Renvoie le nom d'une unité lexicale.
 */
const char* kind_str(const Kind k) {
	if(k == Character) return "Character";
	if(k == Operator) return "Operator";
	if(k == Parenthesis) return "Parenthesis";
	
	fprintf(stderr, "entered unreachable code");
	exit(1);
}


/**
 * Affiche un flux d'unités lexicales sur le flux de sortie standard.
 */
void print_lexemes(const Lexeme *lexemes, size_t n) {
	if(n == 0) {
		printf("[]\n");
	}
	else {
		printf("[");
		
		for(size_t i = 0; i < (n - 1); ++i) {
			printf("%s('%c'), ", kind_str(lexemes[i].token.kind), lexemes[i].token.value);
		}
		
		printf("%s('%c')]\n", kind_str(lexemes[n - 1].token.kind), lexemes[n - 1].token.value);
	}
}


/**
 * Transforme la chaîne de caractères passée en paramètre en un tableau d'unités lexicales.
 */
Lexeme* analyse_lexicale(const char *s, size_t *outLen) {
	if(s == NULL) {
		return NULL;
	}
	
	const size_t len = strlen(s);
	Lexeme *lexemes = checked_malloc(len * sizeof(Lexeme));
	
	size_t i = 0;
	const char *str = s;
	
	while(*s != '\0') {
		if(isalnum(*s)) {
			lexemes[i].token.kind = Character;
			lexemes[i].token.value = *s;
			lexemes[i].index = i;
			++i;
		}
		else if(isop(*s)) {
			lexemes[i].token.kind = Operator;
			lexemes[i].token.value = *s;
			lexemes[i].index = i;
			++i;
		}
		else if(isparenthesis(*s)) {
			lexemes[i].token.kind = Parenthesis;
			lexemes[i].token.value = *s;
			lexemes[i].index = i;
			++i;
		}
		else if(!isspace(*s)) {
			eprintln(str, i, 1);
			fprintf(stderr, "erreur lexicale: lexème inconnu: '%c'\n", *s);
			exit(1);
		}
		
		++s;
	}
	
	*outLen = i;
	return lexemes;
}


/**
 * Déclarations des primitives
 */

#define PRIMITIVE(p) void p (Lexeme *lexemes, size_t n, size_t *i, vstack *stack, const char *s)

PRIMITIVE(Expr);
PRIMITIVE(UnionOp);
PRIMITIVE(UnionVal);
PRIMITIVE(ConcatOp);
PRIMITIVE(ConcatVal);
PRIMITIVE(KleeneOp);
PRIMITIVE(KleeneVal);


/**
 * Définition de l'alphabet
 */
static const char *SIGMA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

/**
 * Transforme un flux d'unités lexicales en un AFN.
 */
AFN analyse_syntaxique(Lexeme *lexemes, size_t n, const char *s) {
	vstack stack = vstack_new();
	
	size_t i = 0;
	Expr(lexemes, n, &i, &stack, s);
	
	if(i < n) {
		eprintln(s, i, n - i);
		fprintf(stderr, "erreur syntaxique: caractères en surplus\n");
		exit(1);
	}
	
	AFN A = vstack_pop(&stack);
	vstack_free(&stack);
	
	return A;
}


/**
 * Définition des primitives
 */

/**
 * Appel d'une primitive avec les paramètres de la primitive courante.
 */
#define INVOKE(p) p (lexemes, n, i, stack, s)

/**
 * Renvoie le caractère du lexème en cours de traitement.
 */
#define PEEK() lexemes[*i].token.value

/**
 * Renvoie `1` s'il y a un lexème courant et si la valeur de ce lexème est égale à `c`, sinon renvoie `0`.
 */
#define PEEK_EQ(c) (*i < n && PEEK() == c)

/**
 * Renvoie `1` s'il y a un lexème courant et que celui-ci représente un `Character`.
 */
#define PEEK_IS_CHAR() (*i < n && lexemes[*i].token.kind == Character)

/**
 * Renvoie `1` s'il y a un lexème courant et que celui-ci peut représenter une concaténation implicite avec le lexème précédent.
 */
#define PEEK_IS_IMPLICIT_CONCAT() (*i < n && (lexemes[*i].token.kind == Character || lexemes[*i].token.value == '('))

/**
 * Déplace le pointeur du lexème courant à `n` lexèmes plus loin.
 */
#define SEEK(n) (*i) += n

PRIMITIVE(Expr) {
	INVOKE(UnionVal);
	INVOKE(UnionOp);
}

PRIMITIVE(UnionOp) {
	if(PEEK_EQ('+')) {
		SEEK(1);
		INVOKE(UnionVal);
		
		AFN rhs = vstack_pop(stack);
		AFN lhs = vstack_pop(stack);
		
		AFN U = afn_union(lhs, rhs);
		
		afn_free(rhs);
		afn_free(lhs);
		
		vstack_push(stack, U);
		
		INVOKE(UnionOp);
	}
}

PRIMITIVE(UnionVal) {
	INVOKE(ConcatVal);
	INVOKE(ConcatOp);
}

PRIMITIVE(ConcatOp) {
	int implicit = 0;
	
	if(PEEK_EQ('.') || (implicit = PEEK_IS_IMPLICIT_CONCAT())) {
		if(!implicit) {
			// skip explicit concat op
			SEEK(1);
		}
		
		INVOKE(ConcatVal);
		
		AFN rhs = vstack_pop(stack);
		AFN lhs = vstack_pop(stack);
		
		AFN C = afn_concat(lhs, rhs);
		
		afn_free(rhs);
		afn_free(lhs);
		
		vstack_push(stack, C);
		
		INVOKE(ConcatOp);
	}
}

PRIMITIVE(ConcatVal) {
	INVOKE(KleeneVal);
	INVOKE(KleeneOp);
}

PRIMITIVE(KleeneOp) {
	while(PEEK_EQ('*')) {
		SEEK(1);
		
		AFN hs = vstack_pop(stack);
		AFN K = afn_kleene(hs);
		
		afn_free(hs);
		
		vstack_push(stack, K);
	}
}

PRIMITIVE(KleeneVal) {
	if(PEEK_EQ('(')) {
		SEEK(1);
		INVOKE(Expr);
		
		if(!PEEK_EQ(')')) {
			eprintln(s, *i, 1);
			fprintf(stderr, "erreur syntaxique: parenthèse fermante attendue\n");
			exit(1);
		}
		
		SEEK(1);
	}
	else {
		if(PEEK_IS_CHAR()) {
			char c = PEEK();
			AFN A = afn_char(c, SIGMA);
			
			vstack_push(stack, A);
			
			SEEK(1);
		}
		else {
			eprintln(s, *i, 1);
			fprintf(stderr, "erreur syntaxique: symbole attendu\n");
			exit(1);
		}
	}
}

/**
 * Transforme la chaîne de caractères spécifiée en un AFN.
 */
AFN compile(const char *s) {
	size_t n;
	Lexeme *lexemes = analyse_lexicale(s, &n);
	
	AFN A = analyse_syntaxique(lexemes, n, s);
	free(lexemes);
	
	return A;
}
