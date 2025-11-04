#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>

#include "util.h"

struct queue {
    void* data;
    uint32_t first;
    uint32_t size;
    uint32_t capacity;
    uint32_t item_size;
};

static inline void queue_init(struct queue* q, const uint32_t item_size) {
    q->data = nullptr;
    q->first = 0;
    q->size = 0;
    q->capacity = 0;
    q->item_size = item_size;
}

static inline void queue_realloc(struct queue* q, const uint32_t new_capacity) {
    if (new_capacity <= q->capacity)
        return;

    void* new_data = malloc(new_capacity * q->item_size);
    if (!new_data)
        panic("queue_realloc: failed to allocate memory");

    if (q->data) {
        if (q->size) {
            if (q->first + q->size < q->capacity) {
                memcpy(new_data, q->data + q->first * q->item_size, q->size * q->item_size);
            } else {
                uint32_t fst_half = (q->capacity - q->first);
                memcpy(new_data, q->data + q->first * q->item_size, fst_half * q->item_size);
                uint32_t snd_half = q->size - fst_half;
                memcpy(new_data + q->first * q->item_size, q->data, snd_half * q->item_size);
            }
        }

        free(q->data);
    }

    q->first = 0;
    q->data = new_data;
    q->capacity = new_capacity;
}

static inline void queue_realloc_if_full(struct queue* q) {
    if (q->size == q->capacity) {
        uint32_t new_capacity = q->capacity == 0 ? 8 : next_power_of_2(q->capacity + 1);
        queue_realloc(q, new_capacity);
    }
}

static inline void queue_push_back(struct queue* q, const void* item) {
    queue_realloc_if_full(q);

    uint32_t next = (q->first + q->size) % q->capacity;
    void* slot = q->data + next * q->item_size;
    memcpy(slot, item, q->item_size);
    q->size++;
}

static inline void* queue_peek_back(struct queue* q) {
    if (q->size == 0)
        return nullptr;

    uint32_t last = (q->first + q->size - 1) % q->capacity;
    return q->data + last * q->item_size;
}

static inline int queue_pop_back(struct queue* q, void* dest) {
    void* slot = queue_peek_back(q);
    if (!slot)
        return 0;

    memcpy(dest, slot, q->item_size);
    q->size--;

    return 1;
}

static inline void queue_push_front(struct queue* q, const void* item) {
    queue_realloc_if_full(q);

    uint32_t next = q->first == 0 ? q->capacity - 1 : q->first - 1;
    void* slot = q->data + next * q->item_size;
    memcpy(slot, item, q->item_size);
    q->first = next;
    q->size++;
}

static inline void* queue_peek_front(struct queue* q) {
    if (q->size == 0)
        return nullptr;

    return q->data + q->first * q->item_size;
}

static inline int queue_pop_front(struct queue* q, void* dest) {
    void* slot = queue_peek_front(q);

    memcpy(dest, slot, q->item_size);
    q->first = (q->first + 1) % q->capacity;
    q->size--;

    return 1;
}

static inline void queue_uninit(struct queue* q) {
    if (q->data) {
        free(q->data);
        q->data = nullptr;
    }
}

#endif
