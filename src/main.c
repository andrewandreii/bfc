#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "parser.h"
#include "code_emitters.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide an input file");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Cannot open file");
        return 1;
    }

    FILE *out = fopen("out.bf", "w");

    token_t *tokens = tokenize(f);

    int i;
    for (i = 0; tokens[i].type != EOF_TOKEN; ++ i) {
        log_token(stdout, tokens + i);
    }

    init_parser();

    ast_node_t *tree = parse_program(tokens);

    code_emitter_context_t *ctx = init_context(out);
    for (i = 0; i < tree->data.program.body_len; ++ i) {
        emit_statement(ctx, tree->data.program.body + i);
    }
}
