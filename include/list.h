#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdio.h>
#include "stdint.h"
#include "util.h"

struct list {
    struct list_node* head;
};

struct list_node {
    void* item;
    struct list_node* next;
};

static inline void list_init(struct list* l) {
    l->head = nullptr;
}

static inline int list_empty(struct list* l) {
    return l->head == nullptr;
}

static inline uint32_t list_len(struct list* l) {
    uint32_t len = 0;
    for (struct list_node* p = l->head; p != nullptr; p = p->next)
        len++;
    return len;
}

static inline void list_push_front(struct list* l, void* item) {
    struct list_node* node = malloc(sizeof(struct list_node));
    if (!node)
        panic("list_push_front: failed to allocate memory");

    node->item = item;
    node->next = l->head;

    l->head = node;
}

static inline int list_pop_front(struct list* l, void** dest) {
    if (list_empty(l))
        return 0;

    struct list_node* p = l->head;
    l->head = p->next;

    *dest = p->item;
    free(p);

    return 1;
}

static inline void list_for_each(struct list* l, mapper f) {
    for (struct list_node* p = l->head; p != nullptr; p = p->next)
        f(p->item);
}

static inline void list_for_each_with_arg(struct list* l, mapper_arg f, void* arg) {
    for (struct list_node* p = l->head; p != nullptr; p = p->next)
        f(p->item, arg);
}

static inline void list_uninit(struct list* l) {
    struct list_node* p = l->head;

    while (p != nullptr) {
        struct list_node* q = p;
        p = p->next;
        free(q);
    }
}

static inline void list_debug(struct list* l) {
    printf("List ");
    for (struct list_node* p = l->head; p != nullptr; p = p->next) {
        printf("%p -> ", p->item);
    }
    printf("(null)\n");
}

#endif
