#ifndef BUILTIN_CODE_EMITTERS_H
#define BUILTIN_CODE_EMITTERS_H

#include <stdio.h>
#include "parser.h"

extern var_t var_table[];
extern struct_t struct_table[];

typedef struct {
    FILE *code_output;
    bfc_value_t *arguments;
    int arguments_length;
    int selected_struct;

    int struct_cell;

    // NOTE: position relative to the struct
    int pos;
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
code_emitter_context_t *init_context(FILE *code_output);
void emit_function_call(code_emitter_context_t *ctx, ast_node_t *func);

// builtins
FUNC_CODE_EMITTER_SIG(read);
FUNC_CODE_EMITTER_SIG(write);
// FUNC_CODE_EMITTER_SIG(while);

// helpers (aka private functions)
FUNC_CODE_EMITTER_SIG(goto);

#endif