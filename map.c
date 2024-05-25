#include "map.h"

static size_t djb2(char *s) {
	size_t hash = 5381;
	int c;

	while ((c = *(s ++))) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

map_t *new_map(int len) {
    map_t *map = malloc(sizeof(map_t));
    map->len = len;
    map->collisions = 0;
    map->buckets = calloc(sizeof(bucket_t), len);

    return map;
}

static entry_t *new_entry(char *key, void *data) {
    entry_t *entry = malloc(sizeof(entry_t));
    entry->key = key;
    entry->data = data;
    entry->next = NULL;
    return entry;
}

static void bucket_append(bucket_t *b, char *key, void *data) {
    if (b->root == NULL) {
        b->root = new_entry(key, data);
        return;
    }

    entry_t *entry;
    for (entry = b->root; entry->next != NULL; entry = entry->next);
    entry->next = new_entry(key, data);
}

static void *bucket_get(bucket_t *b, char *key) {
    if (b->root == NULL) {
        return NULL;
    }

    entry_t *entry;
    for (entry = b->root; strcmp(entry->key, key) && entry->next != NULL; entry = entry->next);

    if (strcmp(entry->key, key) == 0) {
        return entry->data;
    }

    return NULL;
}

static void bucket_set(bucket_t *b, char *key, void *data) {
    if (b->root == NULL) {
        return;
    }

    entry_t *entry;
    for (entry = b->root; strcmp(entry->key, key) && entry->next != NULL; entry = entry->next);

    if (strcmp(entry->key, key) == 0) {
        entry->data = data;
    }

    return;
}


// TODO: write this
static void free_bucket(bucket_t *b) {
}

void *map_get_value(map_t *map, char *key) {
    size_t hash = djb2(key) % map->len;

    entry_t *entry;
    if (entry = bucket_get(map->buckets + hash, key)) {
        return entry->data;
    }

    return NULL;
}

void map_set_value(map_t *map, char *key, void *data) {
    size_t hash = djb2(key) % map->len;

    if (bucket_get(map->buckets + hash, key)) {
        bucket_set(map->buckets + hash, key, data);
        return;
    }

    bucket_append(map->buckets + hash, key, data);
    ++ map->buckets[hash].len;
    ++ map->collisions;
}

void free_map(map_t *map) {
    int i;
    for (i = 0; i < map->len; ++ i) {
        free_bucket(map->buckets + i);
    }

    free(map->buckets);
    free(map);
}