#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "parser.h"

int main() {
    FILE *f = fopen("test/bf_find_example.bfc", "r");

    token_t *tokens = tokenize(f);

    int i;
    for (i = 0; tokens[i].type != EOF_TOKEN; ++ i) {
        log_token(stdout, tokens + i);
    }

    init_parser();

    parse_program(tokens);
}
