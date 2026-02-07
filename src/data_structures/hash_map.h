#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>

struct hash_map_node {
    void *key;
    void *value;
    struct hash_map_node *next;
};

void hash_map_node_init(struct hash_map_node *node, void *key, void *value);
void *hash_map_node_destroy(struct hash_map_node *node);

typedef unsigned int (*hash_map_hash_function)(void *key);
typedef bool (*hash_map_key_compare_function)(void *key_a, void *key_b);
typedef void (*hash_map_for_each_function)(void *key, void *value,
                                           void *context);

struct hash_map {
    int bucket_count;
    unsigned long key_size;
    unsigned long value_size;
    struct hash_map_node **buckets;
    hash_map_hash_function hash_function;
    hash_map_key_compare_function key_compare_function;
    int iter_bucket;
    struct hash_map_node *iter_current_node;
};

// TODO: Maybe restructure ownership so takes pointer and returns when
// overwritten or removed for user to free
// TODO: Dynamic bucket count that auto increases / decreases
void hash_map_init(struct hash_map *map, int bucket_count,
                   unsigned long key_size, unsigned long value_size,
                   hash_map_hash_function hash_function,
                   hash_map_key_compare_function key_compare_function);
void hash_map_destroy(struct hash_map *map);
void *hash_map_put(struct hash_map *map, void *key, void *value);
void *hash_map_get(struct hash_map *map, void *key);
void *hash_map_remove(struct hash_map *map, void *key);
void hash_map_for_each(struct hash_map *map,
                       hash_map_for_each_function function, void *context);

#endif
