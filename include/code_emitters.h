#ifndef BUILTIN_CODE_EMITTERS_H
#define BUILTIN_CODE_EMITTERS_H

#include <stdio.h>
#include "parser.h"

extern var_t var_table[];
extern struct_t struct_table[];

extern char *token_names[];

typedef struct {
    FILE *code_output;
    bfc_value_t *arguments;
    int arguments_length;
    int selected_struct;

    int struct_cell;

    // NOTE: position relative to the struct
    int pos;

    // The aparent position.
    int pivot;

    // used by control flow emitters, they need to know it in order to generate code for their statements
    ast_node_t *node;
} code_emitter_context_t;

typedef int (*code_emitter_func_sig)(code_emitter_context_t *ctx);

typedef struct {
    char *name;

    // TODO: change function signature to include context, arguments etc.
    code_emitter_func_sig func;
} func_table_entry_t;

#define FUNC_CODE_EMITTER_SIG(name) int code_emitter_##name(code_emitter_context_t *ctx)
#define FUNC_CODE_EMITTER_ENTRY(name) { #name, &code_emitter_##name }

// api

// URGENT: context could be a global variable
code_emitter_context_t *init_context(FILE *code_output);
void emit_function_call(code_emitter_context_t *ctx, ast_node_t *func);
void emit_statement(code_emitter_context_t *ctx, ast_node_t *stmt);
int get_usable(int purpose);
bfc_value_t get_suitable_variant(code_emitter_context_t *ctx, int id);
void disable_usable(int id);
void enable_usable(int id);

// builtins
FUNC_CODE_EMITTER_SIG(read);
FUNC_CODE_EMITTER_SIG(write);
FUNC_CODE_EMITTER_SIG(sgoto);
FUNC_CODE_EMITTER_SIG(while);
FUNC_CODE_EMITTER_SIG(if);

// helpers (aka private functions)
FUNC_CODE_EMITTER_SIG(goto);
int _goto(code_emitter_context_t *ctx, bfc_value_t *dest);
int _mov(code_emitter_context_t *ctx, bfc_value_t *dest, bfc_value_t *orig);
int _empty(code_emitter_context_t *ctx, bfc_value_t *cell);
int _add_const(code_emitter_context_t *ctx, bfc_value_t *cell, int c);
int _add(code_emitter_context_t *ctx, bfc_value_t *dest, bfc_value_t *orig);
int _sub_const(code_emitter_context_t *ctx, bfc_value_t *cell, int c);

#endif