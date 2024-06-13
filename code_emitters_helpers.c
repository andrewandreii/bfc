#include "code_emitters.h"

void dump_value(FILE *log, bfc_value_t *val) {
    if (val->var_id == -1) {
        printf("C:%d\n", val->rel_pos);
        return;
    }

    printf("%d:%s%s%d\n", var_table[val->var_id].struct_id, var_table[val->var_id].name, val->rel_pos >= 0 ? " +" : " ", val->rel_pos);
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

extern int usable_var[];
extern size_t usable_var_top;
int get_usable(int purpose) {
    size_t i;
    for (i = 0; i < usable_var_top; ++ i) {
        if (var_table[usable_var[i]].flags & purpose && (var_table[usable_var[i]].flags & ALREADY_IN_USE_FLAG) == 0) {
            return usable_var[i];
        }
    }

    return -1;
}

bfc_value_t get_suitable_variant(code_emitter_context_t *ctx, int id) {
    bfc_value_t all_variants = { id, -1 };
    int before_dist = get_relative_raw_position(ctx->pos, &all_variants);
    printf("before dist: %d\n", before_dist);
    all_variants.rel_pos = 1;
    int after_dist = get_relative_raw_position(ctx->pos, &all_variants);
    printf("after dist: %d\n", after_dist);
    all_variants.rel_pos = 0;
    int current_dist = get_relative_raw_position(ctx->pos, &all_variants);
    printf("current dist: %d\n", current_dist);

    if (current_dist <= before_dist && current_dist <= after_dist) {
        all_variants.rel_pos = 0;
        printf("current won\n");
        return all_variants;
    }
    if (before_dist <= current_dist && before_dist <= after_dist) {
        all_variants.rel_pos = -1;
        printf("before won\n");
        return all_variants;
    }
    // if (after_dist <= current_dist && after_dist <= before_dist) {
    printf("after won\n");
    all_variants.rel_pos = 1;
    return all_variants;
    // }
}

void disable_usable(int id) {
    var_table[id].flags |= ALREADY_IN_USE_FLAG;
}

void enable_usable(int id) {
    var_table[id].flags &= ~ALREADY_IN_USE_FLAG;
}

int can_change(int id) {
    return !(var_table[id].flags & PRESERVE_FLAG);
}

// internal emitters (prefixed with _)

// NOTE: _goto will never actually move us through the struct, only sgoto does that
int _goto(code_emitter_context_t *ctx, bfc_value_t *dest) {
    bfc_value_t temp_dest = *dest;
    printf("rel_pos = %d\n", dest->rel_pos);
    temp_dest.rel_pos += ctx->pivot - ctx->struct_cell;
    printf("actual rel_pos = %d - %d + %d\n", ctx->pivot, ctx->struct_cell, dest->rel_pos);

    int rel_position = get_relative_raw_position(ctx->pos, &temp_dest);
    ctx->pos = var_table[dest->var_id].pos;
    ctx->struct_cell += temp_dest.rel_pos;

    // dump_context(stdout, ctx);

    fputs(var_table[dest->var_id].name, ctx->code_output);

    int dir = rel_position > 0 ? -1 : 1;
    int dir_symbol = rel_position > 0 ? '>' : '<';
    while (rel_position) {
        fputc(dir_symbol, ctx->code_output);
        rel_position += dir;
    }

    return 1;
}

// TODO: use IS_CLEAR_FLAG
int _mov(code_emitter_context_t *ctx, bfc_value_t *dest, bfc_value_t *orig) {
    _goto(ctx, orig);
    fputc('[', ctx->code_output);

    _goto(ctx, dest);
    fputc('+', ctx->code_output);

    _goto(ctx, orig);
    fputc('-', ctx->code_output);

    fputc(']', ctx->code_output);

    return 1;
}

int _empty(code_emitter_context_t *ctx, bfc_value_t *cell) {
    _goto(ctx, cell);
    fputs("[-]", ctx->code_output);

    return 1;
}

// private emitters (follow the code emitter signature, but caan't be used as functions)
FUNC_CODE_EMITTER_SIG(goto) {
    return _goto(ctx, ctx->arguments);
}