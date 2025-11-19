#ifndef IMAGE_STRUCT_H
#define IMAGE_STRUCT_H

#include <stdint.h>

struct image {
    uint8_t* data;
    uint32_t width, height, channels;
};

#endif
