#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

struct vector {
    void* data;
    uint32_t size;
    uint32_t capacity;
    uint32_t item_size;
};

static inline void vec_init(struct vector* vec, const uint32_t item_size) {
    vec->data = nullptr;
    vec->size = 0;
    vec->capacity = 0;
    vec->item_size = item_size;
}

static inline void vec_realloc(struct vector* vec, const uint32_t new_capacity) {
    if (new_capacity <= vec->capacity)
        return;

    void* new_data = realloc(vec->data, new_capacity * vec->item_size);
    if (!new_data)
        panic("vec_realloc: failed to allocate memory");

    vec->data = new_data;
    vec->capacity = new_capacity;
}

static inline void vec_resize(struct vector* vec, const uint32_t new_size) {
    if (new_size > vec->capacity)
        vec_realloc(vec, next_power_of_2(new_size));

    vec->size = new_size;
}

static inline void vec_zero(struct vector* vec, const uint32_t start, const uint32_t end) {
    uint32_t count = end - start;
    if (count > 0) {
        memset(vec->data + start * vec->item_size, 0, count * vec->item_size);
    }
}

static inline void* vec_item(struct vector* vec, const uint32_t idx) {
    if (idx >= vec->size)
        return nullptr;

    return (uint8_t*)vec->data + idx * vec->item_size;
}

static inline int vec_get(struct vector* vec, const uint32_t idx, void* dest) {
    void* item = vec_item(vec, idx);
    if (!item)
        return 0;

    memcpy(dest, item, vec->item_size);
    return 1;
}

static inline int vec_set(struct vector* vec, const uint32_t idx, const void* item) {
    void* slot = vec_item(vec, idx);
    if (!slot)
        return 0;

    memcpy(slot, item, vec->item_size);
    return 1;
}

static inline void vec_push(struct vector* vec, const void* item) {
    if (vec->size == vec->capacity) {
        uint32_t new_capacity = vec->capacity == 0 ? 8 : next_power_of_2(vec->capacity + 1);

        vec_realloc(vec, new_capacity);
    }

    void* slot = (uint8_t*)vec->data + vec->size * vec->item_size;
    memcpy(slot, item, vec->item_size);

    vec->size += 1;
}

static inline int vec_pop(struct vector* vec, void* dest) {
    if (vec->size == 0)
        return 0;

    vec_get(vec, vec->size - 1, dest);

    vec->size -= 1;

    return 1;
}

static inline void* vec_front(struct vector* vec) {
    return vec_item(vec, 0);
}

static inline void* vec_back(struct vector* vec) {
    if (vec->size == 0)
        return nullptr;

    return vec_item(vec, vec->size - 1);
}

static inline void vec_for_each(struct vector* vec, mapper f) {
    for (uint32_t i = 0; i < vec->size; i++) {
        void* p = (uint8_t*)vec->data + i * vec->item_size;
        f(p);
    }
}

static inline void vec_for_each_with_arg(struct vector* vec, mapper_arg f, void* arg) {
    for (uint32_t i = 0; i < vec->size; i++) {
        void* p = (uint8_t*)vec->data + i * vec->item_size;
        f(p, arg);
    }
}

static inline void vec_clone(const struct vector* src, struct vector* dest) {
    dest->data = nullptr;
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->item_size = src->item_size;

    if (src->capacity > 0) {
        void* data = malloc(src->capacity * src->item_size);
        if (!data)
            panic("vec_clone: failed to allocate memory");

        memcpy(data, src->data, src->size * src->item_size);

        dest->data = data;
    }
}

static inline void vec_uninit(struct vector* vec) {
    if (vec->data) {
        free(vec->data);
        vec->data = nullptr;
    }
}

static inline void vec_debug(struct vector* vec) {
    printf("Vector size = %d capacity = %d item_size = %d\n", vec->size, vec->capacity, vec->item_size);
}

#endif
