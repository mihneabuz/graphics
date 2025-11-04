#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "util.h"
#include "vector.h"

struct map {
    struct vector buckets;
    uint32_t size;
    comparator cmp;
    hasher hash;
};

struct map_entry {
    void* key;
    void* item;
    uint64_t hash;
};

static inline int uint_comparator(void* e1, void* e2) {
    return (int64_t)e2 - (int64_t)e1;
}

static inline uint64_t uint_hasher(void* e) {
    return (uint64_t)e * 5381;
}

static inline int str_comparator(void* e1, void* e2) {
    return strcmp(e1, e2);
}

static inline uint64_t str_hasher(void* e) {
    unsigned char* str = e;
    uint64_t hash = 5381, c = 0;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

static inline void map_init(struct map* map, comparator cmp, hasher hash) {
    vec_init(&map->buckets, sizeof(struct list));
    map->size = 0;
    map->cmp = cmp;
    map->hash = hash;
}

static inline void map_realloc(struct map* map, uint32_t new_capacity) {
    uint32_t old_capacity = map->buckets.capacity;

    vec_resize(&map->buckets, new_capacity);
    vec_zero(&map->buckets, old_capacity, map->buckets.capacity);

    for (uint32_t idx = 0; idx < old_capacity; idx++) {
        struct list* bucket = vec_item(&map->buckets, idx);

        while (bucket->head) {
            struct map_entry* first = bucket->head->item;
            uint32_t new_idx = first->hash % map->buckets.capacity;

            if (new_idx > idx) {
                struct list_node* node = bucket->head;
                bucket->head = node->next;

                struct list* new_bucket = vec_item(&map->buckets, new_idx);
                node->next = new_bucket->head;
                new_bucket->head = node;
            } else {
                break;
            }
        }

        struct list_node* p = bucket->head;
        while (p != nullptr) {
            struct list_node* q = p->next;
            if (!q)
                break;

            struct map_entry* entry = q->item;
            uint32_t new_idx = entry->hash % map->buckets.capacity;

            if (new_idx > idx) {
                p->next = q->next;

                struct list* new_bucket = vec_item(&map->buckets, new_idx);
                q->next = new_bucket->head;
                new_bucket->head = q;
            } else {
                p = q;
            }
        }
    }
}

static inline struct map_entry* map_insert(struct map* map, void* key, void* item) {
    if (map->buckets.capacity == 0 || map->size >= map->buckets.capacity * 0.8) {
        uint32_t new_capacity = map->buckets.capacity == 0 ? 32 : next_power_of_2(map->buckets.capacity + 1);
        map_realloc(map, new_capacity);
    }

    uint64_t hash = map->hash(key);

    struct map_entry* entry = malloc(sizeof(struct map_entry));
    if (!entry)
        panic("map_insert: failed to allocate memory");

    entry->key = key;
    entry->item = item;
    entry->hash = hash;

    uint32_t idx = hash % map->buckets.capacity;
    struct list* bucket = vec_item(&map->buckets, idx);

    for (struct list_node* p = bucket->head; p != nullptr; p = p->next) {
        struct map_entry* other = p->item;
        if (hash == other->hash && map->cmp(key, other->key) == 0) {
            p->item = entry;
            return other;
        }
    }

    list_push_front(bucket, entry);
    map->size += 1;
    return nullptr;
}

static inline struct map_entry* map_find(struct map* map, void* key) {
    uint64_t hash = map->hash(key);
    uint32_t idx = hash % map->buckets.capacity;
    struct list* bucket = vec_item(&map->buckets, idx);

    for (struct list_node* p = bucket->head; p != nullptr; p = p->next) {
        struct map_entry* entry = p->item;
        if (hash == entry->hash && map->cmp(key, entry->key) == 0) {
            return entry;
        }
    }

    return nullptr;
}

static inline int map_remove(struct map* map, void* key) {
    uint64_t hash = map->hash(key);
    uint32_t idx = hash % map->buckets.capacity;
    struct list* bucket = vec_item(&map->buckets, idx);

    struct list_node* p = bucket->head;
    if (p) {
        struct map_entry* entry = p->item;
        if (hash == entry->hash && map->cmp(key, entry->key) == 0) {
            bucket->head = p->next;
            free(entry);
            free(p);
            map->size--;
            return 1;
        }
    }

    while (p) {
        struct list_node* q = p->next;
        if (!q)
            break;

        struct map_entry* entry = q->item;
        if (hash == entry->hash && map->cmp(key, entry->key) == 0) {
            p->next = q->next;
            free(entry);
            free(q);
            map->size--;
            return 1;
        } else {
            p = q;
        }
    }

    return 0;
}

static inline void _map_bucket_mapper(void* bucket, void* arg) {
    list_for_each(bucket, arg);
}

static inline void map_for_each(struct map* map, mapper f) {
    vec_for_each_with_arg(&map->buckets, _map_bucket_mapper, f);
}

static inline void map_uninit(struct map* map) {
    map_for_each(map, free);
    vec_for_each(&map->buckets, (mapper)list_uninit);
    vec_uninit(&map->buckets);
}

static inline void _map_debug_entry(void* entry) {
    struct map_entry* e = entry;
    printf(" %lu -> %lu\n", (uint64_t)e->key, (uint64_t)e->item);
}

static inline void map_debug(struct map* map) {
    printf("Map size = %d capacity = %d\n", map->size, map->buckets.capacity);
    map_for_each(map, _map_debug_entry);
}

#endif
