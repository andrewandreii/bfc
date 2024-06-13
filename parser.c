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


// TODO: maybe less than this
int usable_var[MAX_DEFINED_VARS];
size_t usable_var_top = 0;

typedef struct { char *name; void *func; } func_table_entry_t;
extern func_table_entry_t builtin_functions[];
extern const size_t builtin_function_count;
// size_t builtin_function_count = 1;

void init_parser(void) {
    var_map = new_map(MAX_DEFINED_VARS / 2);
    struct_map = new_map(MAX_DEFINED_VARS / 2);
    func_map = new_map(30);

    int i;
    for (i = 0; i < builtin_function_count; ++ i) {
        map_set_value(func_map, builtin_functions[i].name, builtin_functions + i);
    }
}

extern const char *token_names[];
void expect_token(token_t *t, enum token_type type) {
    if (t->type != type) {
        // TODO: add info about location in file
        // fprintf(stderr, "Expected token of type: %s not %s.\n", token_names[type], token_names[t->type]);
        // exit(-1);
        expected_token_error(t, type);
    }
}

#define expect_tokens2(token, type1, type2) expect_tokens(token, 2, type1, type2)
void expect_tokens(token_t *t, int type_num, ...) {
    va_list type;
    va_start(type, type_num);

    int ok = 0;

    enum token_type current_type;

    int i;
    for (i = 0; i < type_num; ++ i) {
        if (t->type == (current_type = va_arg(type, enum token_type))) {
            ok = 1;
            break;
        }
    }

    va_end(type);

    if (!ok) {
        // fprintf(stderr, "Expected token of type: %s not %s.\n", token_names[current_type], token_names[t->type]);
        // exit(-1);
        expected_token_error(t, current_type);
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

int get_struct_by_name(char *name) {
    struct_t *struct_ = map_get_value(struct_map, name);

    if (!struct_) {
        return -1;
    }

    return struct_ - struct_table;
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

/*
A var list is a comma separated sequence of arguments, that might have default values or hints.
A var list declares all the variables it contains.
*/
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

bfc_value_t get_bfc_values_from_expr(expr_t *expr) {
    bfc_value_t lvar = {-1, 0};

    if (expr->type == -1) {
        lvar.var_id = get_var_by_name(expr->left->val.str);
        if (lvar.var_id == -1) {
            lvar.var_id = get_struct_by_name(expr->left->val.str);
            if (lvar.var_id == -1) {
                UNDEFINED_VARIABLE_ERROR(expr->left);
            } else {
                // URGENT: the struct needs to know its children or at least the first child
            }
        }
        return lvar;
    }

    int sign = 1;
    while (expr) {
        if (expr->left->type == ID) {
            if (lvar.var_id != -1) {
                REL_POS_MULTIPLE_ANCHORS_ERROR(expr->left);
            }
            lvar.var_id = get_var_by_name(expr->left->val.str);
            if (lvar.var_id == -1) {
                lvar.var_id = get_struct_by_name(expr->left->val.str);
                if (lvar.var_id == -1) {
                    UNDEFINED_VARIABLE_ERROR(expr->left);
                } else {
                    // URGENT: the struct needs to know its children or at least the first child
                }
            }
        } else {
            lvar.rel_pos += expr->left->val.num * sign;
        }

        sign = expr->type == ADD ? 1 : -1;
        expr = expr->right;
    }

    return lvar;
}

expr_t *parse_expr(token_t **t) {
    //expect_tokens(*t, 3, ID, NUM, BINOP);
    if ((*t)->type != ID && (*t)->type != NUM && (*t)->type != BINOP) {
        return NULL;
    }

    int sign = 1;
    if ((*t)->type == BINOP) {
        switch ((*t)->val.num) {
            case '+': break;
            case '-': sign = -1; break;
            default: UNEXPECTED_TOKEN(*t);
        }
        ++ *t;
    }

    expr_t *expr = malloc(sizeof(expr_t));
    expr->right = NULL;
    if ((*t)->type == ID || (*t)->type == NUM) {
        expr->left = *t;
        expr->left->val.num *= sign;
    } else {
        expr->type = -1;
        return expr;
    }

    ++ *t;
    switch ((*t)->val.num) {
        case '+': {
            expr->type = ADD;
        } break;
        case '-': {
            expr->type = SUB;
        } break;
        default: {
            expr->type = -1;
            return expr;
        }
    }

    ++ *t;
    expr->right = parse_expr(t);

    return expr;
}

// TODO: make it possible to specify the type of the argument
/*
Parses a comma separated list of expressions.
Currently, the default argument type is variable, so a process is unable to return or take a function type.
*/
bfc_value_t *parse_id_list(token_t **t, int *count) {
    int len = 10;
    bfc_value_t *vars = malloc(sizeof(bfc_value_t) * len);
    int top = 0;

    while (1) {
        if (top >= len) {
            len *= 2;
            vars = realloc(vars, len);
        }

        expr_t *expr = parse_expr(t);
        if (!expr) {
            break;
        }

        vars[top] = get_bfc_values_from_expr(expr);
        // vars[top] = get_var_by_name((*t)->val.str);
        // if (var_ids[top] == -1) {
        //     var_ids[top] = get_func_by_name((*t)->val.str);
        //     if (var_ids[top] != -1) {
        //         var_ids[top] = register_var((*t)->val.str, -1, 0);
        //     }
        // }
        ++ top;

        // if (struct_id != -1) {
        // set_var_flags(var_id, DEFINED_FLAG);
        // }

        if ((*t)->type != COMMA) {
            break;
        }
        ++ *t;
    }

    *count = top;
    return vars;
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
    if ((*t)->type == L_BRACKET) {
        ++ *t;
        expect_token(*t, NUM);
        struct_span = (*t)->val.num;
        ++ *t;
        expect_token(*t, R_BRACKET);
        ++ *t;
    }

    // NOTE: start_address is calculated at the end of parsing when all struct have been added

    return register_struct(name, var_count, struct_span);
}

/*
Parses a statement. The type of stmt is -1 when it encounters a ELSE
 */
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

            stmt->data.call.vars = parse_id_list(t, &(stmt->data.call.arg_len));
        } break;
        case REF: {
            stmt->type = REF_STMT;
            ++ *t;
            stmt->data.call.vars = parse_id_list(t, &(stmt->data.call.arg_len));
        } break;
        case L_BRACKET: {
            ++ *t;
            expect_token(*t, L_BRACKET);
            ++ *t;

            stmt->type = COMPILER_DIRECTIVE;

            // stmt->data.call.vars = parse_id_list(t, &(stmt->data.call.arg_len));
            token_t *comp_dir_start = *t;
            int comp_dir_len = 0;
            while ((*t)->type != R_BRACKET && (*t)->type != NL && (*t)->type != EOF_TOKEN) {
                switch ((*t)->type) {
                    case USEIF: {
                        (*t)->type = USEIF_FLAG;
                    } break;
                    case USECONST: {
                        (*t)->type = USECONST_FLAG;
                    } break;
                    case USECPY: {
                        (*t)->type = USECPY_FLAG;
                    } break;
                    case USEA: {
                        (*t)->type = USEA_FLAG;
                    } break;
                    default: {
                        UNEXPECTED_TOKEN(*t);
                    }
                }

                ++ *t;
                ++ comp_dir_len;
            }

            if (comp_dir_len == 0) {
                UNEXPECTED_TOKEN(*t);
            }

            expect_token(*t, R_BRACKET);
            ++ *t;
            expect_token(*t, R_BRACKET);
            ++ *t;

            while ((*t)->type == NL) {
                ++ *t;
            }

            expect_token(*t, REF);
            ++ *t;
            while ((*t)->type == ID) {
                int id = get_var_by_name((*t)->val.str);
                if (id == -1) {
                    UNDEFINED_VARIABLE_ERROR(*t);
                }

                if ((var_table[id].flags & (USEIF_FLAG | USECONST_FLAG | USECPY_FLAG | USEA_FLAG)) == 0) {
                    usable_var[usable_var_top] = id;
                    ++ usable_var_top;
#ifdef DEBUG
                } else {
                    fprintf(stderr, "Variable already inside of usable_var.\n");
#endif
                }

                int i;
                for (i = 0; i < comp_dir_len; ++ i) {
                    var_table[id].flags |= comp_dir_start[i].type;
                    printf("set %d flag to %s\n", comp_dir_start[i].type, (*t)->val.str);
                }
                
                ++ *t;
            }
        } break;
        case WHILE: {
            stmt->type = CONTROL_FLOW_STMT;
            // NOTE: may be used for lambdas, macros??
            // stmt->data.control_flow_call.func_id = ;
            ++ *t;
            stmt->data.control_flow_call.func_id = get_func_by_name("while");
            stmt->data.control_flow_call.vars = parse_id_list(t, &(stmt->data.control_flow_call.arg_len));
            stmt->data.control_flow_call.body = parse_body(t, &stmt->data.control_flow_call.body_len);

            expect_token(*t, END);
            ++ *t;
        } break;
        case IF: {
            stmt->type = CONTROL_FLOW_STMT;

            ++ *t;
            stmt->data.control_flow_call.func_id = get_func_by_name("if");
            stmt->data.control_flow_call.vars = parse_id_list(t, &(stmt->data.control_flow_call.arg_len));
            stmt->data.control_flow_call.body = parse_body(t, &stmt->data.control_flow_call.body_len);

            if ((*t)->type == ELSE) {
                ++ *t;
                stmt->data.control_flow_call.body2 = parse_body(t, &stmt->data.control_flow_call.body2_len);
            }

            expect_token(*t, END);
            ++ *t;
        } break;
        case ELSE: {
            stmt->type = -1;
        } break;
        default: {
            UNEXPECTED_TOKEN(*t);
        }
    }
}

ast_node_t *parse_body(token_t **t, size_t *count) {
    // TODO: maybe the length can be guessed by the number of lines
    int len = 10;
    ast_node_t *body = malloc(len * sizeof(ast_node_t));
    int top = 0;
    
    while ((*t)->type == NL) {
        ++ *t;
    }

    while ((*t)->type != EOF_TOKEN && (*t)->type != END) {
        if (top >= len) {
            len *= 2;
            body = realloc(body, len);
        }

        parse_statement(t, body + top);
        if (body[top].type == -1) {
            break;
        }
        ++ top;
    
        while ((*t)->type == NL) {
            ++ *t;
        }
    }

    *count = top;
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

    prog->data.program.body = parse_body(&t, &prog->data.program.body_len);

    return prog;
}