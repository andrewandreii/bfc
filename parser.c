#include "parser.h"

#define MAX_DEFINED_VARS 1000
#define MAX_DEFINED_STRUCTS 100
var_t var_table[MAX_DEFINED_VARS];
size_t var_table_top = 0;
struct_t struct_table[MAX_DEFINED_STRUCTS];
size_t struct_table_top = 0;

map_t *var_map;
map_t *struct_map;
map_t *func_map;

// TODO: move to antoher file
int code_emitter_read(FILE *stream) {
    fputc(',', stream);

    return 1;
}

typedef int (*code_emitter_func_sig)(FILE *);
typedef struct {
    char *name;

    // TODO: change function signature to include context, arguments etc.
    code_emitter_func_sig func;
} func_table_entry_t;

func_table_entry_t builtin_functions[] = {
    { "read", &code_emitter_read }
};


// TODO: maybe less than this
int usable_var[MAX_DEFINED_VARS];


void init_parser(void) {
    var_map = new_map(MAX_DEFINED_VARS / 2);
    struct_map = new_map(MAX_DEFINED_VARS / 2);
    func_map = new_map(30);

    int i;
    for (i = 0; i < sizeof(builtin_functions) / sizeof(*builtin_functions); ++ i) {
        map_set_value(func_map, builtin_functions[i].name, builtin_functions + i);
    }
}


extern const char *token_names[];
void expect_token(token_t *t, enum token_type type) {
    if (t->type != type) {
        // TODO: add info about location in file
        fprintf(stderr, "Expected token of type: %s not %s.\n", token_names[type], token_names[t->type]);
        exit(-1);
    }
}

int get_next_struct_id() {
    return struct_table_top;
}

int register_struct(char *name, int self_size, int length) {
    struct_t *s = struct_table + struct_table_top;
    
    if (name) {
        strcpy(s->name, name);
        map_set_value(struct_map, s->name, s);
    } else {
        s->name[0] = '\0';
    }

    s->self_size = self_size;
    s->length = length;

    fprintf(stderr, "Registered struct %s\n", s->name);

    return struct_table_top ++;
}

void set_var_flags(int var_id, int flags) {
    var_table[var_id].flags |= flags;
}

int get_var_flag(int var_id, int flag) {
    return var_table[var_id].flags & flag;
}

int get_var_by_name(char *name) {
    var_t *var = map_get_value(var_map, name);

    if (!var) {
        return -1;
    }

    return var - var_table;
}

int get_func_by_name(char *name) {
    func_table_entry_t *func = map_get_value(func_map, name);

    if (!func) {
        return -1;
    }

    fprintf(stderr, "Found function %s\n", name);

    return func - builtin_functions;
}

// TODO: use name
int register_var(char *name, int struct_id, int relative_pos) {
    var_t *v = var_table + var_table_top;

    strcpy(v->name, name);
    map_set_value(var_map, v->name, v);

    v->struct_id = struct_id;
    v->pos = relative_pos;
    v->flags = 0;

    fprintf(stderr, "Registered variable %s\n", name);

    return var_table_top ++;
}

int parse_var_list(token_t **t, int struct_id) {
    int count = 0;
    while (1) {
        expect_token(*t, ID);

        int var_id = get_var_by_name((*t)->val.str);
        if (var_id == -1) {
            var_id = register_var((*t)->val.str, struct_id, count);
        }

        // if (struct_id != -1) {
        set_var_flags(var_id, DEFINED_FLAG);
        // }

        ++ count;

        ++ *t;
        if ((*t)->type != COMMA) {
            break;
        }
        ++ *t;
    }

    return count;
}

// TODO: make it possible to specify the type of the argument
/*
Parses a comma separated list of ids (functions or variables).
Currently, the default argument type is variable, so a process is unable to return or take a function type.
*/
int *parse_id_list(token_t **t, int *count) {
    int len = 10;
    int *var_ids = malloc(sizeof(int) * len);
    int top = 0;

    while (1) {
        if ((*t)->type == NUM) {
            // URGENT: add possibility for numbers as arguments
        }

        expect_token(*t, ID);

        if (top >= len) {
            len *= 2;
            var_ids = realloc(var_ids, len);
        }

        var_ids[top] = get_var_by_name((*t)->val.str);
        if (var_ids[top] == -1) {
            var_ids[top] = get_func_by_name((*t)->val.str);
            if (var_ids[top] != -1) {
                var_ids[top] = register_var((*t)->val.str, -1, 0);
            }
        }
        ++ top;

        // if (struct_id != -1) {
        // set_var_flags(var_id, DEFINED_FLAG);
        // }

        ++ *t;
        if ((*t)->type != COMMA) {
            break;
        }
        ++ *t;
    }

    *count = top;
    return var_ids;
}

// Parses and registers struct and it's variables
int parse_struct(token_t **t) {
    ++ (*t);

    char *name = NULL;
    if ((*t)->type == ID) {
        name = (*t)->val.str;
        ++ *t;
    }

    expect_token(*t, L_PARAN);
    ++ *t;

    int var_count = parse_var_list(t, get_next_struct_id());
    
    expect_token(*t, R_PARAN);
    ++ *t;

    int struct_span;
    if ((*t)->type == R_BRACKET) {
        ++ *t;
        expect_token(*t, NUM);
        struct_span = (*t)->val.num;
    }

    // NOTE: start_address is calculated at the end of parsing when all struct have been added

    return register_struct(name, var_count, struct_span);
}

void parse_statement(token_t **t, ast_node_t *stmt) {
    switch ((*t)->type) {
        case STRUCT: {
            stmt->type = STRUCT_STMT;
            stmt->data.struct_.struct_id = parse_struct(t);
        } break;
        case ID: {
            stmt->type = FUNC_CALL;

            stmt->data.call.func_id = get_func_by_name((*t)->val.str);
            ++ *t;

            stmt->data.call.var_id = parse_id_list(t, &(stmt->data.call.arg_len));
        } break;
        case REF: {
            stmt->type = REF_STMT;
            ++ *t;
            stmt->data.call.var_id = parse_id_list(t, &(stmt->data.call.arg_len));
        } break;
        case L_BRACKET: {
            ++ *t;
            expect_token(*t, L_BRACKET);
            ++ *t;

            stmt->type = COMPILER_DIRECTIVE;

            stmt->data.call.var_id = parse_id_list(t, &(stmt->data.call.arg_len));
        
            expect_token(*t, R_BRACKET);
            ++ *t;
            expect_token(*t, R_BRACKET);
            ++ *t;
        } break;
        case WHILE: {
            stmt->type = WHILE_STMT;
            // NOTE: may be used for lambdas, macros??
            // stmt->data.control_flow_call.func_id = ;
            ++ *t;
            stmt->data.control_flow_call.var_id = parse_id_list(t, &(stmt->data.control_flow_call.arg_len));
            stmt->data.control_flow_call.body = parse_body(t);
        }
        default: {
            printf("Unexpected start of statement token %s.\n", token_names[(*t)->type]);
            exit(1);
        }
    }
}

ast_node_t *parse_body(token_t **t) {
    // TODO: maybe the length can be guessed by the number of lines
    int len = 10;
    ast_node_t *body = malloc(len * sizeof(ast_node_t));
    int top = 0;
    
    while ((*t)->type != EOF_TOKEN && (*t)->type != END) {
        while ((*t)->type == NL) {
            ++ *t;
        }

        if (top >= len) {
            len *= 2;
            body = realloc(body, len);
        }

        parse_statement(t, body + top);
        ++ top;
    }

    return body;
}

ast_node_t *parse_proc(token_t **t) {
    return NULL; // TODO: implement
}

ast_node_t *parse_program(token_t *t) {
    ast_node_t *prog = malloc(sizeof(ast_node_t));
    prog->type = PROGRAM;

    if (t->type == PROC) {
        prog->data.program.decl = parse_proc(&t);
    }

    prog->data.program.body = parse_body(&t);

    return prog;
}