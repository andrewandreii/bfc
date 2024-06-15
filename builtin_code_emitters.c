#include "code_emitters.h"


func_table_entry_t builtin_functions[] = {
    FUNC_CODE_EMITTER_ENTRY(read),
    FUNC_CODE_EMITTER_ENTRY(write),
    // control flow is currently handled in the same way as functions
    FUNC_CODE_EMITTER_ENTRY(while),
    FUNC_CODE_EMITTER_ENTRY(if)
};
const size_t builtin_function_count = sizeof(builtin_functions) / sizeof(*builtin_functions);

extern int usable_var[];
extern size_t usable_var_top;

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

// TODO: this could be written with a while, not sure if it's worth it
FUNC_CODE_EMITTER_SIG(mov) {
    

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

    int temp1_id = -1;
    bfc_value_t suitable_temp1;
    if (ctx->node->data.control_flow_call.body2_len > 0) {
        temp1_id = get_usable(USEIF_FLAG);
        if (temp1_id == -1) {
            EXPECTED_USABLE_VARIABLE(ctx->node->start);
        }
        disable_usable(temp1_id);
        suitable_temp1 = get_suitable_variant(ctx, temp1_id);
        _empty(ctx, &suitable_temp1);
        _add_const(ctx, &suitable_temp1, 1);
    }

    int temp0_id = get_usable(USECPY_FLAG);
    if (temp0_id == -1) {
        EXPECTED_USABLE_VARIABLE(ctx->node->start);
    }

    disable_usable(temp0_id);

    // URGENT: needs more optimization based on where we will be in the future
    // for example:
    // if x+1
    //     read y+1
    // end
    // should choose a temp variable in the next cell
    // could be solved by introducing `with` and leaving it up to the user
    bfc_value_t suitable_temp0 = get_suitable_variant(ctx, temp0_id);
    _empty(ctx, &suitable_temp0);

    // ctx->arguments->rel_pos += suitable_temp0.rel_pos == -1 ? 1 : suitable_temp0.rel_pos == 1 ? -1 : 0;

    _mov(ctx, &suitable_temp0, ctx->arguments);

    _goto(ctx, &suitable_temp0);
    fputc('[', ctx->code_output);
    _empty(ctx, &suitable_temp0);

    ast_node_t *body = ctx->node->data.control_flow_call.body;
    size_t body_len = ctx->node->data.control_flow_call.body_len;

    int i;
    for (i = 0; i < body_len; ++ i) {
        emit_statement(ctx, body + i);
    }

    if (temp1_id != -1) {
        _goto(ctx, &suitable_temp1);
        _sub_const(ctx, &suitable_temp1, 1);
    }

    _goto(ctx, &suitable_temp0);
    fputc(']', ctx->code_output);

    enable_usable(temp0_id);

    if (temp1_id != -1) {
        _goto(ctx, &suitable_temp1);
        fputc('[', ctx->code_output);
        _empty(ctx, &suitable_temp1);
        for (i = 0; i < body_len; ++ i) {
            emit_statement(ctx, body + i);
        }
        _goto(ctx, &suitable_temp1);
        fputc(']', ctx->code_output);
    }

    return 1;
}