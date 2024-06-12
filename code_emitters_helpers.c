#include "code_emitters.h"

void dump_value(FILE *log, bfc_value_t *val) {
    if (val->var_id == -1) {
        printf("C:%d\n", val->rel_pos);
        return;
    }

    printf("%d:%s+%d\n", var_table[val->var_id].struct_id, var_table[val->var_id].name, val->rel_pos);
}

void dump_context(FILE *log, code_emitter_context_t *ctx) {
    printf("Current context:\n");
    printf("Struct id: %d\n", ctx->selected_struct);
    printf("Struct cell no.: %d\n", ctx->struct_cell);
    printf("Variable no.: %d\n", ctx->pos);
    printf("Arguments:\n");

    int i;
    for (i = 0; i < ctx->arguments_length; ++ i) {
        putchar('\t');
        dump_value(log, ctx->arguments + i);
    }
}

int get_relative_raw_position(int position, bfc_value_t *var) {
#ifdef DEBUG
    if (var->var_id == -1) {
        fprintf(stderr, "get_raw_position: Not a variable.\n");
        exit(-1);
    }
#endif

    return (var_table[var->var_id].pos + struct_table[var_table[var->var_id].struct_id].self_size * var->rel_pos) - position;
}

FUNC_CODE_EMITTER_SIG(goto) {
    int rel_position = get_relative_raw_position(ctx->pos, ctx->arguments);
    ctx->pos = var_table[ctx->arguments->var_id].pos;
    ctx->struct_cell += ctx->arguments->rel_pos;

    dump_context(stdout, ctx);

    int dir = rel_position > 0 ? -1 : 1;
    int dir_symbol = rel_position > 0 ? '>' : '<';
    while (rel_position) {
        fputc(dir_symbol, ctx->code_output);
        rel_position += dir;
    }

    return 1;
}