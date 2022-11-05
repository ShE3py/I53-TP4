#include "compregex.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
			eprintln(s, i, 1);
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

#define PRIMITIVE(p) void p (Lexeme *lexemes, size_t n, size_t *i, AFN *A, const char *s)
#define INVOKE(p) p (lexemes, n, i, A, s)
#define PEEK_EQ(c) (*i < n && lexemes[*i].token.value == c)
#define PEEK_IS_CHAR (*i < n && lexemes[*i].token.kind == Character)
#define SEEK(n) (*i) += n

PRIMITIVE(Expr);
PRIMITIVE(UnionOp);
PRIMITIVE(UnionVal);
PRIMITIVE(ConcatOp);
PRIMITIVE(ConcatVal);
PRIMITIVE(KleeneOp);
PRIMITIVE(KleeneVal);


/**
 * Transforme un flux d'unités lexicales en un AFN.
 */
AFN analyse_syntaxique(Lexeme *lexemes, size_t n, const char *s) {
	const int q = 0;
	AFN A = afn_init(q, 1, &q, 1, &q, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	
	size_t i = 0;
	Expr(lexemes, n, &i, &A, s);
	
	if(i < n) {
		eprintln(s, i, n - i);
		fprintf(stderr, "erreur syntaxique: caractères en surplus\n");
		exit(1);
	}
	
	return A;
}


PRIMITIVE(Expr) {
	INVOKE(UnionVal);
	INVOKE(UnionOp);
}

PRIMITIVE(UnionOp) {
	if(PEEK_EQ('+')) {
		SEEK(1);
		INVOKE(UnionVal);
		INVOKE(UnionOp);
	}
}

PRIMITIVE(UnionVal) {
	INVOKE(ConcatVal);
	INVOKE(ConcatOp);
}

PRIMITIVE(ConcatOp) {
	if(PEEK_EQ('.')) {
		SEEK(1);
		INVOKE(ConcatVal);
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
		if(PEEK_IS_CHAR) {
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
