#ifndef SYMBOL_H
#define SYMBOL_H

#include "tokenizer.h"

// SEMANTIC FLAGS
#define USEIF_FLAG    1
// used for arithmetic and constant assignment
#define USECONST_FLAG (1 << 1)
#define USECPY_FLAG   (1 << 2)
#define USEA_FLAG     (1 << 3)
#define ALREADY_IN_USE_FLAG (1 << 4)
// should this var be preserved? it's on by default
#define PRESERVE_FLAG (1 << 5)

// HELPER FLAGS
#define DEFINED_FLAG (1 << 6)
// useful for optimizations, so that we don't unnecessarily clear cells
// TODO: implement it in parser.h and keep track of it in the code emitters
#define IS_CLEAR_FLAG (1 << 7)

/*
Any variable in this laangues must have a struct to which it belongs to, a position relative to the struct and modifiers.
*/
typedef struct {
    // NOTE: might be optional once temporary variables become automated
    /*
    The name of the variable.
    */
    char name[ID_LEN];

    /*
    Position of the structure to which this belongs to in the struct list.
    */
    int struct_id;

    /*
    Position relative to the structure

    example:
        position 1 in the context of
        
        ```
        proc (n) -> x
        struct (x, y)[n]
        ```
        y is on any of the positions 1, 3, 5...
    */
    int pos;

    /*
    `useif`, `useconst` etc.
    knowing them separately can be nice for readibility
    of the code and also for keeping track of compatibilities
    */
    int flags;
} var_t;

/*
Represents an array of structs, not just one.
*/
typedef struct {
    /*
    The name of the struct (optional).
    */
    char name[ID_LEN];

    /*
    The number of variables one struct contains.
    */
    int self_size;

    /*
    The length of the array of structs. If `length` == -1, then the struct array is infinite.
    */
    int length;

    /*
    The cell index on which this struct starts.
    */
    int start_address;
} struct_t;

#endif