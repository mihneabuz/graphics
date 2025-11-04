#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define nullptr NULL

#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_NORMAL "\033[0m"

typedef int (*comparator)(void*, void*);
typedef uint64_t (*hasher)(void*);
typedef void (*mapper)(void*);
typedef void (*mapper_arg)(void*, void*);

static inline void panic(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, C_RED "Panicked with error:\n");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n" C_NORMAL);
    va_end(args);
    exit(1);
}

static inline void check_ptr(void* ptr) {
    if (ptr == nullptr)
        panic("check_ptr failed: nullptr");
}

static inline uint32_t next_power_of_2(uint32_t size) {
    if (size == 0)
        return 1;

    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;

    return size;
}

#endif
