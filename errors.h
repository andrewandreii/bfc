#ifndef ERRORS_H
#define ERRORS_H

// TODO: this is a brief sketch of the error system, the process of writing defines should be automated
// see lang_errors
#define UNDEFINED_VARIABLE_ERROR \
    do { \
        fprintf(stderr, "UNDEFINED VARIABLE: Encountered a variable that hasn't been defined.\n"); \
        exit(-1); \
    } while (0)

#endif