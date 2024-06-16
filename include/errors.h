#ifndef ERRORS_H
#define ERRORS_H

#define expected_token_error(got, expected) \
    do {{ \
        fprintf(stderr, "Error encountered at line %d, col %d\n", got->source_file_pos.line_no, (got)->source_file_pos.col_no); \
        fprintf(stderr, "Expected token of type: %s not %s.\n", token_names[expected], token_names[(got)->type]); \
        exit(-1); \
    }} while (0)

#define UNDEFINED_VARIABLE_ERROR(t) \
    do { \
        fprintf(stderr, "Error encountered at line %d, col %d (%s)\n", (t)->source_file_pos.line_no, (t)->source_file_pos.col_no, token_names[(t)->type]); \
        fprintf(stderr, "UNDEFINED_VARIABLE_ERROR: Encountered a variable that hasn't been defined."); \
        exit(-1); \
    } while (0)

#define REL_POS_MULTIPLE_ANCHORS_ERROR(t) \
    do { \
        fprintf(stderr, "Error encountered at line %d, col %d (%s)\n", (t)->source_file_pos.line_no, (t)->source_file_pos.col_no, token_names[(t)->type]); \
        fprintf(stderr, "REL_POS_MULTIPLE_ANCHORS_ERROR: When specifying a cell address, it cannot depend on multiple variables/structures."); \
        exit(-1); \
    } while (0)

#define UNEXPECTED_TOKEN(t) \
    do { \
        fprintf(stderr, "Error encountered at line %d, col %d (%s)\n", (t)->source_file_pos.line_no, (t)->source_file_pos.col_no, token_names[(t)->type]); \
        fprintf(stderr, "UNEXPECTED_TOKEN: This type of token wasn't expected at this position."); \
        exit(-1); \
    } while (0)

#define CANT_FIND_FUNCTION_SIGNATURE(t) \
    do { \
        fprintf(stderr, "Error encountered at line %d, col %d (%s)\n", (t)->source_file_pos.line_no, (t)->source_file_pos.col_no, token_names[(t)->type]); \
        fprintf(stderr, "CANT_FIND_FUNCTION_SIGNATURE: The number of arguments does not match any signature."); \
        exit(-1); \
    } while (0)

#define EXPECTED_USABLE_VARIABLE(t) \
    do { \
        fprintf(stderr, "Error encountered at line %d, col %d (%s)\n", (t)->source_file_pos.line_no, (t)->source_file_pos.col_no, token_names[(t)->type]); \
        fprintf(stderr, "EXPECTED_USABLE_VARIABLE: There is currently no usable variable that satisfies the needs of this statement."); \
        exit(-1); \
    } while (0)

#endif