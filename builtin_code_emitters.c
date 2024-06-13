#include "code_emitters.h"


func_table_entry_t builtin_functions[] = {
    FUNC_CODE_EMITTER_ENTRY(read),
    FUNC_CODE_EMITTER_ENTRY(write),
    // control flow is currently handled in the same way as functions
    FUNC_CODE_EMITTER_ENTRY(while),
    FUNC_CODE_EMITTER_ENTRY(if)
};
const size_t builtin_function_count = sizeof(builtin_functions) / sizeof(*builtin_functions);


FUNC_CODE_EMITTER_SIG(read) {
    int i;
    for (i = 0; i < ctx->arguments_length; ++ i) {
        code_emitter_goto(ctx);
        fputc(',', ctx->code_output);
        ++ ctx->arguments;
    }

    return 1;
}

FUNC_CODE_EMITTER_SIG(write) {
    int i;
    for (i = 0; i < ctx->arguments_length; ++ i) {
        code_emitter_goto(ctx);
        fputc('.', ctx->code_output);
        ++ ctx->arguments;
    }

    return 1;
}

FUNC_CODE_EMITTER_SIG(while) {
    if (ctx->arguments_length != 1) {
        // URGENT: while must know its own position
        // CANT_FIND_FUNCTION_SIGNATURE(NULL);
        exit(123);
    }

    bfc_value_t *starting_position = ctx->arguments;
    ast_node_t *body = ctx->node->data.control_flow_call.body;
    size_t body_len = ctx->node->data.control_flow_call.body_len;
    code_emitter_goto(ctx);
    fputc('[', ctx->code_output);

    int i;
    for (i = 0; i < body_len; ++ i) {
        emit_statement(ctx, body + i);
    }

    ctx->arguments = starting_position;
    ctx->arguments_length = 1;
    code_emitter_goto(ctx);
    fputc(']', ctx->code_output);

    return 1;
}

FUNC_CODE_EMITTER_SIG(if) {
    // URGENT: automate functions signature checking
    code_emitter_goto(ctx);

    if (ctx->node->data.control_flow_call.body2_len > 0) {

    }

    return 1;
}