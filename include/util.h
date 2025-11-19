#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define nullptr NULL

#define true 1
#define false 0

#define unused(x) (void)x;

#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_NORMAL "\033[0m"

typedef void (*callback)();
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

static inline void warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "WARN: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static inline void check_ptr(void* ptr) {
    if (ptr == nullptr)
        panic("check_ptr failed: nullptr");
}

static inline uint32_t flip_bytes(uint32_t x) {
    uint32_t mask = 255, y = 0;

    y |= ((x & mask) << 24);
    y |= ((x & mask << 8) << 8);
    y |= ((x & mask << 16) >> 8);
    y |= ((x & mask << 24) >> 24);

    return y;
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

static inline char* read_file(const char* path) {
    int ret;

    FILE* file = fopen(path, "r");
    if (!file)
        return nullptr;

    ret = fseek(file, 0, SEEK_END);
    if (ret != 0)
        return nullptr;

    uint32_t size = ftell(file);

    ret = fseek(file, 0, SEEK_SET);
    if (ret != 0)
        return nullptr;

    char* buffer = malloc((size_t)size + 1);
    if (!buffer)
        panic("read_file: failed to allocate memory");

    fread(buffer, size, 1, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

#endif
