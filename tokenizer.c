#include "tokenizer.h"

const char *token_names[] = {
	"PROC", "STRUCT", "REF", "WHILE", "IF", "ELSE", "END", "NL",
	"L_PRAN", "R_PARAN", "L_BRACKET", "R_BRACKET", "EQUAL",
	"COMMA", "ARROW", "IF0", "NUM", "ID", "EOF_TOKEN", "BINOP"
};

void log_token(FILE *outfile, token_t *t) {
    // if type < NUM => does not use val
    if (t->type < NUM) {
        fprintf(outfile, "%s%c", token_names[t->type], t->type == NL ? '\n' : ' ');
    } else if (t->type == NUM) {
        fprintf(outfile, "(%s: %d) ", token_names[t->type], t->val.num);
    } else if (t->type == ID) {
        fprintf(outfile, "(%s: %s) ", token_names[t->type], t->val.str);
    } else if (t->type == BINOP) {
        fprintf(outfile, "(%s: %c) ", token_names[t->type], t->val.num);
    } else {
        fprintf(outfile, "(Unknown token type: %d) ", t->type);
    }
}

const char *keywords[] = { "proc", "struct", "ref", "while", "if", "else", "end", "if0", NULL };
const enum token_type keyword_str_to_type[] = { PROC, STRUCT, REF, WHILE, IF, ELSE, END, IF0 };

token_t *tokenize(FILE *stream) {
    token_t *t_list = malloc(sizeof(token_t) * TOKEN_BUF_LEN);
    int t_top = 0;
    int t_len = TOKEN_BUF_LEN;

    char id_buffer[ID_LEN];
    int id_len = ID_LEN;
    char c;

    int char_no = 0;
    int line_no = 1;

    while (!feof(stream)) {
        while (isspace(c = fgetc(stream)) && c != '\n') {
            ++ char_no;
        }

        if (c == EOF) {
            break;
        }

        char next_c = fgetc(stream);
        if (next_c == EOF) {
            fseek(stream, -1, SEEK_CUR);
        } else {
            fseek(stream, -2, SEEK_CUR);
        }

        if (t_top >= t_len) {
            t_list = realloc(t_list, sizeof(token_t) * (t_len + TOKEN_BUF_INC));
            t_len += TOKEN_BUF_INC;
        }

        t_list[t_top].source_file_pos = (source_file_pos_t){ .line_no = line_no, .col_no = char_no };

        if (isdigit(c) /* || ((c == '+' || c == '-') && isdigit(next_c)) */) {
            if (!isdigit(c)) {
                fgetc(stream);
            }

            int sign = 1;
            if (c == '-') {
                sign = -1;
            }

            t_list[t_top].type = NUM;

            int num = 0;
            while (isdigit(c = fgetc(stream))) {
                num = num * 10 + (c - '0');
                ++ char_no;
            }
            fseek(stream, -1, SEEK_CUR);

            t_list[t_top].val.num = num * sign;
            ++ t_top;
        } else if (isalpha(c)) {
            int id_top = 0;
            while ((isalnum(c = fgetc(stream)))) {
                id_buffer[id_top] = c;
                ++ id_top;
            }
            id_buffer[id_top] = '\0';

            char_no += strlen(id_buffer);

            if (c != EOF) fseek(stream, -1, SEEK_CUR);

            int is_keyword = 0;
            int i;
            for (i = 0; keywords[i]; ++ i) {
                if (id_buffer[0] == keywords[i][0] && !strcmp(id_buffer, keywords[i])) {
                    t_list[t_top].type = keyword_str_to_type[i];
                    is_keyword = 1;
                    break;
                }
            }

            if (!is_keyword) {
                t_list[t_top].type = ID;
                t_list[t_top].val.str = malloc(t_top + 1);
                strcpy(t_list[t_top].val.str, id_buffer);
            }
            ++ t_top;
        } else {
            ++ char_no;
            enum token_type type;

            if (c == '/' && next_c == '/') {
                char_no = 0;
                ++ line_no;
                while ((c = fgetc(stream)) != '\n' && !feof(stream));
                goto found_token;
            } else if (c == '-' && next_c == '>') {
                ++ char_no;
                type = ARROW;
                goto found_token;
            }

            switch (fgetc(stream)) {
                case '\n': {
                    ++ line_no;
                    char_no = 0;
                    type = NL;
                } break;
                case '(': {
                    type = L_PARAN;
                } break;
                case ')': {
                    type = R_PARAN;
                } break;
                case '[': {
                    type = L_BRACKET;
                } break;
                case ']': {
                    type = R_BRACKET;
                } break;
                case '=': {
                    type = EQUAL;
                } break;
                case ',': {
                    type = COMMA;
                } break;
                case '-': case '/':
                case '+': case '*': {
                    type = BINOP;
                    t_list[t_top].val.num = c;
                } break;
                default: {
                error:
                    // TODO: make the error better
                    fprintf(
                        stderr,
                        "Error in token starting with: \'%c\' (0x%02X) (line %d, char %d)\n",
                        c, c, line_no, char_no
                    );
                    exit(1);
                }
            }

found_token:
            t_list[t_top].type = type;
            ++ t_top;
        }
        // log_token(stdout, t_list + t_top - 1);
        // fflush(stdout);
        // printf("token ended at %d.\n", ftell(stream));
    }

    t_list[t_top].type = EOF_TOKEN;

    return t_list;
}