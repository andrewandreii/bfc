#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tokenizer.h"
#include "symbol.h"
#include "map.h"
#include "errors.h"

enum ast_node_type {
    // ID_LIST == VAR_LIST
    PROGRAM, PROC_DECL, ID_LIST,
    STRUCT_STMT, CONTROL_FLOW_STMT,
    FUNC_CALL,

    // maybe used in the future for bfc debugging
    COMPILER_DIRECTIVE, REF_STMT
};

/*
A bfc value refers to:
    - a constant (i.e. 1 or 'a')
    - a variable (i.e. the variable X of struct Y)
    - (in the future) a procedure (i.e. the procedure X from file Y)
 */
typedef struct {
    // if -1, the struct refers to a constant
    int var_id;

    // if var_id is -1, it refers to the constant value
    int rel_pos;
} bfc_value_t;

/*
grammar:
EXPR := (ID|NUM) (PLUS|MINUS) EXPR | (ID|NUM)

parsing function: `parse_expr`
*/
typedef struct _expr_t {
    /*
    if (char)type is negative, the struct is a binary expression
    if (char)type is positive, the struct is of type token_t
    */
    enum { ADD = 0xfe, SUB } type;
    token_t *left;
    struct _expr_t *right;
} expr_t;

typedef struct _ast_node_t {
    enum ast_node_type type;
    union {
        /*
        The root node of bfc programs.

        grammar:
        PROGRAM := PROC_DECL? STATEMENT*

        parsing function: `parse_program`
         */
        struct {
            struct _ast_node_t *decl;
            struct _ast_node_t *body;
            size_t body_len;
        } program;
        /*
        Keeps track of the signature of bfc files (useful when calling into other files).
        TODO: make description clearer
        
        grammar:
        PROC_DECL := PROC L_PARAN ID_LIST R_PARAN ARROW ID

        parsing function: `parse_proc`
         */
        struct {
            int *var_id;
            int ret_var_id;
        } proc;
        // NOTE: only for debugging purposes
        /*
        Keeps track of which struct was defined where. Useless for anything else other than debugging.

        grammar:
        STRUCT_STMT := STRUCT L_PARAN VAR_LIST R_PARAN VEC_SIZE?

        parsing function: `parse_struct`
         */
        struct {
            int struct_id;
        } struct_;
        /*
        Normal functions.
        The `func_id` is tied to a code_emitter function that receives the current context (like struct position) and a list of variables.
        `func_id` signature should be `void code_emitter(FILE *stream, semantic_context *ctx, int *var_ids);`

        grammar:
        FUNC_CALL := ID ID_LIST

        parsing function: `parse_statement` 
         */
        struct {
            int func_id;
            bfc_value_t *vars;
            int arg_len;
        } call;
        /*
        Control flow statements (ie. if/if0, while/while0 etc. - see `lang_keywords.md`)
        They behave the same as normal calls, but the function with `func_id` also receives a list of statements (`body`).
        `func_id` signature should be `void code_emitter(FILE *stream, semantic_context *ctx, int *var_ids, ast_node_t *body_stmts);`

        NOTE: the `type` member of the parent already specifies built-in control flow statements, this would only be useful for custom code emitters.
         
        grammar:
        WHILE_STMT := WHILE ID NL STATEMENT* END
        IF_STMT := IF ID NL STATEMENT* (ELSE NL STATEMENT*) END
        
        parsing function:
         */
        struct {
            int func_id;
            bfc_value_t *vars;
            int arg_len;
            struct _ast_node_t *body;
            size_t body_len;
            // Secondary body, used by the if-else statement
            struct _ast_node_t *body2;
            size_t body2_len;
        } control_flow_call;
        expr_t expr;
    } data;
} ast_node_t;

// parse compiler directive, it doens't have a node
// [[useif usecomp]]
// ref a, b
// transforms to
// useif a, b
// usecomp a, b

void init_parser(void);
ast_node_t *parse_program(token_t *tokens);
ast_node_t *parse_body(token_t **t, size_t *count);
void parse_statement(token_t **t, ast_node_t *stmt);

#endif