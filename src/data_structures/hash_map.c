#include "hash_map.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void hash_map_node_init(struct hash_map_node *node, void *key, void *value) {
    node->key = key;
    node->value = value;
    node->next = NULL;
}

void *hash_map_node_destroy(struct hash_map_node *node) {
    free(node->key);
    return node->value;
}

void hash_map_init(struct hash_map *map, int bucket_count,
                   unsigned long key_size, unsigned long value_size,
                   hash_map_hash_function hash_function) {
    map->bucket_count = bucket_count;
    map->key_size = key_size;
    map->value_size = value_size;
    map->hash_function = hash_function;
    map->buckets = malloc(bucket_count * sizeof(struct hash_map_node *));
    map->iter_bucket = 0;
    map->iter_current_node = NULL;

    for (int i = 0; i < bucket_count; i++) {
        map->buckets[i] = NULL;
    }
}

void hash_map_destroy(struct hash_map *map) {
    for (int i = 0; i < map->bucket_count; i++) {
        struct hash_map_node *current_node = map->buckets[i];

        while (current_node) {
            struct hash_map_node *next_node = current_node->next;
            hash_map_node_destroy(current_node);
            free(current_node);
            current_node = next_node;
        }
    }

    free(map->buckets);
}

void *hash_map_put(struct hash_map *map, void *key, void *value) {
    // Should avoid copying them maybe implement arena allocator
    void *owned_key = malloc(map->key_size);
    memcpy(owned_key, key, map->key_size);

    // void *owned_value = malloc(map->value_size);
    // memcpy(owned_value, value, map->value_size);

    int index = map->hash_function(owned_key) % map->bucket_count;
    struct hash_map_node *bucket_head = map->buckets[index];

    if (!bucket_head) {
        struct hash_map_node *new_node = malloc(sizeof(struct hash_map_node));
        hash_map_node_init(new_node, owned_key, value);
        map->buckets[index] = new_node;
        return NULL;
    }

    struct hash_map_node *current_node = bucket_head;

    while (current_node) {
        if (memcmp(current_node->key, owned_key, map->key_size) == 0) {
            struct hash_map_node *next_node = current_node->next;
            void *old_value = hash_map_node_destroy(current_node);
            hash_map_node_init(current_node, owned_key, value);
            current_node->next = next_node;
            return old_value;
        }

        if (!current_node->next) {
            struct hash_map_node *new_node =
                malloc(sizeof(struct hash_map_node));
            hash_map_node_init(new_node, owned_key, value);
            new_node->next = NULL;
            current_node->next = new_node;
            return NULL;
        }

        current_node = current_node->next;
    }

    return NULL;
}

void *hash_map_get(struct hash_map *map, void *key) {
    int index = map->hash_function(key) % map->bucket_count;

    struct hash_map_node *current_node = map->buckets[index];

    while (current_node) {
        if (memcmp(current_node->key, key, map->key_size) == 0) {
            return current_node->value;
        }

        current_node = current_node->next;
    }

    return NULL;
}

void *hash_map_remove(struct hash_map *map, void *key) {
    int index = map->hash_function(key) % map->bucket_count;

    struct hash_map_node *current_node = map->buckets[index];
    struct hash_map_node *previous_node = NULL;

    while (current_node) {
        struct hash_map_node *next_node = current_node->next;

        if (memcmp(current_node->key, key, map->key_size) == 0) {
            void *value = hash_map_node_destroy(current_node);
            free(current_node);

            if (previous_node) {
                previous_node->next = next_node;
            } else {
                map->buckets[index] = next_node;
            }

            return value;
        }

        previous_node = current_node;
        current_node = next_node;
    }

    return NULL;
}

void hash_map_for_each(struct hash_map *map,
                       hash_map_for_each_function function, void *context) {
    for (int i = 0; i < map->bucket_count; i++) {
        struct hash_map_node *current_node = map->buckets[i];

        while (current_node) {
            function(current_node->key, current_node->value, context);
            current_node = current_node->next;
        }
    }
}
