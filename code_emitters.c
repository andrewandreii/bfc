#include "code_emitters.h"

extern func_table_entry_t builtin_functions[];

code_emitter_context_t *init_context(FILE *code_output) {
    code_emitter_context_t *ctx = malloc(sizeof(code_emitter_context_t));

    ctx->arguments_length = 0;
    ctx->code_output = code_output;
    ctx->pos = ctx->selected_struct = ctx->struct_cell = 0;

    return ctx;
}

void emit_function_call(code_emitter_context_t *ctx, ast_node_t *func) {
    ctx->arguments = func->data.call.vars;
    ctx->arguments_length = func->data.call.arg_len;

    builtin_functions[func->data.call.func_id].func(ctx);
}

void emit_control_flow(code_emitter_context_t *ctx, ast_node_t *stmt) {
    ctx->arguments = stmt->data.control_flow_call.vars;
    ctx->arguments_length = stmt->data.control_flow_call.arg_len;
}

void emit_statement(code_emitter_context_t *ctx, ast_node_t *stmt) {
    switch (stmt->type) {
        case FUNC_CALL: {
            emit_function_call(ctx, stmt);
        } break;
        case CONTROL_FLOW_STMT: {
            emit_control_flow(ctx, stmt);
        } break;
        default: {
            printf("Cannot emit for: %d\n", stmt->type);
        }
    }
}