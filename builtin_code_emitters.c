#include "code_emitters.h"


func_table_entry_t builtin_functions[] = {
    FUNC_CODE_EMITTER_ENTRY(read),
    FUNC_CODE_EMITTER_ENTRY(write)
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