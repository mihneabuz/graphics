#ifndef MSTRING_H
#define MSTRING_H

#include "vector.h"

#define TERMINATOR '\0'

struct string {
    struct vector inner;
};

static inline void string_init(struct string* str) {
    vec_init(&str->inner, 1);
}

static inline char* string_ptr(struct string* str) {
    return str->inner.data;
}

static inline uint32_t string_len(struct string* str) {
    return str->inner.size;
}

static inline struct vector* string_as_vec(struct string* str) {
    return &str->inner;
}

static inline void string_add_terminator(struct string* str) {
    char term = TERMINATOR;
    vec_push(&str->inner, &term);
}

static inline void string_strip_terminator(struct string* str) {
    char* back = vec_back(&str->inner);
    if (back && *back == TERMINATOR)
        str->inner.size -= 1;
}

static inline void string_push(struct string* str, char c) {
    string_strip_terminator(str);
    vec_push(&str->inner, &c);
    string_add_terminator(str);
}

static inline void string_append(struct string* str, const char* s, uint32_t count) {
    string_strip_terminator(str);
    vec_extend(&str->inner, s, count);
    string_add_terminator(str);
}

static inline void string_pop(struct string* str, uint32_t count) {
    string_strip_terminator(str);

    if (count > str->inner.size)
        count = str->inner.size;

    str->inner.size -= count;
    string_add_terminator(str);
}

static inline void string_uninit(struct string* str) {
    vec_uninit(&str->inner);
}

#endif
