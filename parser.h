#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "symbol.h"
#include "map.h"

enum ast_node_type {
    // ID_LIST == VAR_LIST
    PROGRAM, PROC_DECL, ID_LIST,
    STRUCT_STMT, WHILE_STMT, IF_STMT,
    FUNC_CALL,

    // maybe used in the future for bfc debugging
    COMPILER_DIRECTIVE, REF_STMT
};

typedef struct {
    enum { NUM, VAR } type;
    int val;
} arg_t;

typedef struct _ast_node_t {
    enum ast_node_type type;
    union {
        /*
            The root node of bfc programs.
         */
        struct {
            struct _ast_node_t *decl;
            struct _ast_node_t *body;
        } program;
        /*
            Keeps track of the signature of bfc files (useful when calling into other files).
            TODO: make description clearer
         */
        struct {
            int *var_id;
            int ret_var_id;
        } proc;
        // NOTE: only for debugging purposes
        /*
            Keeps track of which struct was defined where. Useless for anything else other than debugging.
         */
        struct {
            // struct _ast_node_t *id_list;
            // int size;
            int struct_id;
        } struct_;
        /*
            Normal functions.
            The `func_id` is tied to a code_emitter function that receives the current context (like struct position) and a list of variables.
            `func_id` signature should be `void code_emitter(FILE *stream, semantic_context *ctx, int *var_ids);`
         */
        struct {
            // char *name;
            // struct _ast_node_t *id_list;
            int func_id;
            int *var_id;
            int arg_len;
        } call;
        /*
            Control flow statements (ie. if/if0, while/while0 etc. - see `lang_keywords.md`)
            They behave the same as normal calls, but the function with `func_id` also receives a list of statements (`body`).
            `func_id` signature should be `void code_emitter(FILE *stream, semantic_context *ctx, int *var_ids, ast_node_t *body_stmts);`

            NOTE: the `type` member of the parent already specifies built-in control flow statements, this would only be useful for custom code emitters.
         */
        struct {
            int func_id;
            int *var_id;
            int arg_len;
            struct _ast_node_t *body;
        } control_flow_call;
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
ast_node_t *parse_body(token_t **t);
void parse_statement(token_t **t, ast_node_t *stmt);

#endif