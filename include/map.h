#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _entry_t {
    char *key;
    void *data;
    struct _entry_t *next;
} entry_t;

typedef struct {
    int len;
    entry_t *root;
} bucket_t;

#define MAX_COLLISIONS 5
typedef struct {
    int len;
    int collisions;
    bucket_t *buckets;
} map_t;

map_t *new_map(int len);
void *map_get_value(map_t *map, char *key);
void map_set_value(map_t *map, char *key, void *data);
void free_map(map_t *map);

#endif