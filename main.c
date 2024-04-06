#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

int main() {
    FILE *f = fopen("bf_test.bfc", "r");
    token_t *tokens = tokenize(f);

    int i;
    for (i = 0; tokens[i].type != -1; ++ i) {
        log_token(stdout, tokens + i);
    }
}
