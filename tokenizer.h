#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum token_type {
	PROC, STRUCT, REF, WHILE, IF, ELSE, END, NL,
	L_PARAN, R_PARAN, L_BRACKET, R_BRACKET, EQUAL,
	COMMA, ARROW, IF0, NUM, ID, EOF_TOKEN,
	BINOP
};

typedef struct {
	enum token_type type;
	union {
		char *str;
		int num;
	} val;
} token_t;

#define TOKEN_BUF_LEN 10
#define TOKEN_BUF_INC 100

#define ID_LEN 30
#define ID_INC 15

token_t *tokenize(FILE *stream);
void log_token(FILE *outfile, token_t *t);

#endif
