#ifndef ERRORS_H
#define ERRORS_H

#define UNDEFINED_VARIABLE_ERROR \
    do { \
        fprintf(stderr, "UNDEFINED_VARIABLE_ERROR: Encountered a variable that hasn't been defined."); \
        exit(-1); \
    } while (0)

#define REL_POS_MULTIPLE_ANCHORS_ERROR \
    do { \
        fprintf(stderr, "REL_POS_MULTIPLE_ANCHORS_ERROR: When specifying a cell address, it cannot depend on multiple variables/structures."); \
        exit(-1); \
    } while (0)

#endif