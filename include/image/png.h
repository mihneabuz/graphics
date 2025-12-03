#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#include "compress.h"
#include "util.h"
#include "vector.h"

#include "image/struct.h"

uint8_t png_sig[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

enum png_color_spec { SRGB = 1 };

enum png_color_type {
    TRUECOLOR = 1,
    TRUECOLOR_A,
    GRAYSCALE,
    GRAYSCALE_A,
    INDEXED,
};

struct png_parser_state {
    uint32_t width, height, channels;

    uint8_t bit_depth;
    uint8_t color_type;
    enum png_color_spec color_spec;

    struct vector palette;
    struct vector encoded_data;

    int done;
};

static inline void png_parser_state_init(struct png_parser_state* state) {
    state->width = 0;
    state->height = 0;
    state->channels = 0;
    state->bit_depth = 0;
    state->color_type = 0;
    state->color_spec = SRGB;
    state->done = 0;
    vec_init(&state->encoded_data, 1);
    vec_init(&state->palette, 1);
}

static inline void png_parser_state_uninit(struct png_parser_state* state) {
    vec_uninit(&state->encoded_data);
    vec_uninit(&state->palette);
}

// https://www.w3.org/TR/png-3/#5Chunk-layout
struct png_chunk {
    uint32_t size;
    uint8_t type[4];
    uint8_t* data;
    uint32_t crc;
};

static inline void png_read_chunk(uint8_t* raw, struct png_chunk* chunk) {
    chunk->size = flip_bytes(*(uint32_t*)raw);
    memcpy(chunk->type, raw + 4, 4);
    chunk->data = raw + 8;
    chunk->crc = flip_bytes(*(uint32_t*)(raw + 8 + chunk->size));
}

static inline void png_advance_chunk(struct png_chunk* chunk) {
    png_read_chunk(chunk->data + chunk->size + 4, chunk);
}

// https://www.w3.org/TR/png-3/#11IHDR
static inline int png_parse_ihdr_chunk(struct png_chunk* chunk, struct png_parser_state* state) {
    state->width = flip_bytes(*(uint32_t*)chunk->data);
    state->height = flip_bytes(*(uint32_t*)(chunk->data + 4));
    state->bit_depth = *(uint8_t*)(chunk->data + 8);

    uint8_t color_type = *(uint8_t*)(chunk->data + 9);
    switch (color_type) {
        case 0:  // Grayscale
            state->channels = 3;
            state->color_type = GRAYSCALE;
            break;

        case 2:  // Truecolor
            state->channels = 3;
            state->color_type = TRUECOLOR;
            break;

        case 3:  // Indexed-color
            state->channels = 3;
            state->color_type = INDEXED;
            break;

        case 4:  // Grayscale with alpha
            state->channels = 4;
            state->color_type = GRAYSCALE_A;
            break;

        case 6:  // Truecolor with alpha
            state->channels = 4;
            state->color_type = TRUECOLOR_A;
            break;
    }

    uint8_t compression = *(uint8_t*)(chunk->data + 12);
    if (compression != 0) {
        warn("PNG file has unsupported compression method %d", compression);
        return 0;
    }

    uint8_t filter = *(uint8_t*)(chunk->data + 11);
    if (filter != 0) {
        warn("PNG file has unsupported filter method %d", filter);
        return 0;
    }

    uint8_t interlace = *(uint8_t*)(chunk->data + 12);
    if (interlace != 0) {
        warn("PNG file has unsupported interlace method %d", interlace);
        return 0;
    }

    return 1;
}

// https://www.w3.org/TR/png-3/#11IDAT
static inline int png_parse_idat_chunk(struct png_chunk* chunk, struct png_parser_state* state) {
    vec_extend(&state->encoded_data, chunk->data, chunk->size);
    return 1;
}

// https://www.w3.org/TR/png-3/#11IEND
static inline int png_parse_iend_chunk(struct png_chunk* chunk, struct png_parser_state* state) {
    unused(chunk);
    state->done = 1;
    return 1;
}

// https://www.w3.org/TR/png-3/#11PLTE
static inline int png_parse_plte_chunk(struct png_chunk* chunk, struct png_parser_state* state) {
    if (chunk->size % 3 != 0)
        return 0;

    vec_extend(&state->palette, chunk->data, chunk->size);
    return 1;
}

// https://www.w3.org/TR/png-3/#srgb-standard-colour-space
static inline int png_parse_srgb_chunk(struct png_chunk* chunk, struct png_parser_state* state) {
    unused(chunk);
    state->color_spec = SRGB;
    return 1;
}

struct png_chunk_loader {
    const char* type;
    int (*parser)(struct png_chunk* chunk, struct png_parser_state* state);
};

static inline int png_parse_chunk(struct png_chunk* chunk, struct png_parser_state* state) {
    struct png_chunk_loader loaders[] = {
        {"IHDR", png_parse_ihdr_chunk}, {"IDAT", png_parse_idat_chunk},
        {"IEND", png_parse_iend_chunk}, {"PLTE", png_parse_plte_chunk},
        {"sRGB", png_parse_srgb_chunk},
    };

    for (uint32_t i = 0; i < sizeof(loaders) / sizeof(struct png_chunk_loader); i++) {
        struct png_chunk_loader* l = &loaders[i];
        if (memcmp(chunk->type, l->type, 4) == 0)
            return l->parser(chunk, state);
    }

    return 1;
}

static inline int png_read(uint8_t* raw, struct png_parser_state* state) {
    if (strncmp((const char*)raw, (const char*)png_sig, sizeof(png_sig)) != 0)
        return 0;

    struct png_chunk chunk;
    png_read_chunk(raw + 8, &chunk);

    while (1) {
        if (!png_parse_chunk(&chunk, state))
            return 0;

        if (state->done)
            break;

        png_advance_chunk(&chunk);
    }

    return 1;
}

// https://www.w3.org/TR/png-3/#9Filter-type-4-Paeth
static inline uint8_t png_paeth(uint8_t a, uint8_t b, uint8_t c) {
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);

    if (pa <= pb && pa <= pc)
        return a;

    if (pb <= pc)
        return b;

    return c;
}

// https://www.w3.org/TR/png-3/#9Filter-types
static inline void png_unfilter_line(uint8_t* out,
                                     uint8_t* line,
                                     uint8_t* prev,
                                     uint32_t length,
                                     uint32_t bpp) {
    uint8_t filter = *line;
    uint8_t* scanline = line + 1;

    switch (filter) {
        case 0:  // None
            memcpy(out, scanline, length);
            break;

        case 1:  // Sub
            for (uint32_t i = 0; i < length; i++) {
                uint8_t a = i >= bpp ? out[i - bpp] : 0;
                out[i] = scanline[i] + a;
            }
            break;

        case 2:  // Up
            for (uint32_t i = 0; i < length; i++) {
                uint8_t b = prev ? prev[i] : 0;
                out[i] = scanline[i] + b;
            }
            break;

        case 3:  // Average
            for (uint32_t i = 0; i < length; i++) {
                uint8_t a = i >= bpp ? out[i - bpp] : 0;
                uint8_t b = prev ? prev[i] : 0;
                out[i] = scanline[i] + ((a + b) / 2);
            }
            break;

        case 4:  // Paeth
            for (uint32_t i = 0; i < length; i++) {
                uint8_t a = i >= bpp ? out[i - bpp] : 0;
                uint8_t b = prev ? prev[i] : 0;
                uint8_t c = (prev && i >= bpp) ? prev[i - bpp] : 0;
                out[i] = scanline[i] + png_paeth(a, b, c);
            }
            break;
    }
}

static inline void png_load(struct png_parser_state* state, struct image* img) {
    img->width = state->width;
    img->height = state->height;
    img->channels = state->channels;

    uint32_t img_size = img->width * img->height * img->channels;
    if (img_size == 0)
        return;

    img->data = calloc(img_size, 1);
    if (!img->data)
        panic("png_load: failed to allocate memory");

    uint32_t inflated_size = img_size + img->height;
    uint8_t* inflated_data = calloc(inflated_size, 1);
    if (!inflated_data)
        panic("png_load: failed to allocate memory");

    zlib_inflate(inflated_data, inflated_size, state->encoded_data.data, state->encoded_data.size);

    enum png_color_type type = state->color_type;

    uint32_t bpp = 0;
    uint8_t* temp = nullptr;
    uint8_t* temp_prev = nullptr;
    switch (type) {
        case GRAYSCALE:
        case INDEXED:
            bpp = 1;
            temp = malloc(img->width);
            temp_prev = malloc(img->width);
            break;

        case GRAYSCALE_A:
            bpp = 2;
            temp = malloc(img->width * 2);
            temp_prev = malloc(img->width * 2);
            break;

        default:
            bpp = img->channels;
    }

    // TODO: handle different bit_depth

    uint8_t* prev = nullptr;
    for (uint32_t i = 0; i < img->height; i++) {
        uint8_t* line = &inflated_data[i * (img->width * bpp + 1)];
        uint8_t* img_line = &img->data[(img->height - 1 - i) * img->width * img->channels];

        switch (type) {
            case TRUECOLOR:
            case TRUECOLOR_A:
                png_unfilter_line(img_line, line, prev, img->width * bpp, bpp);
                prev = img_line;
                break;

            case GRAYSCALE:
            case GRAYSCALE_A:
                png_unfilter_line(temp, line, prev, img->width * bpp, bpp);
                for (uint32_t j = 0; j < img->width; j++) {
                    uint8_t* p = &img_line[j * img->channels];
                    p[0] = temp[j];
                    p[1] = temp[j];
                    p[2] = temp[j];
                }
                prev = temp;
                temp = temp_prev;
                temp_prev = prev;
                break;

            case INDEXED:
                // TODO: handle color palette
                break;
        }
    }

    if (temp)
        free(temp);

    if (temp_prev)
        free(temp_prev);

    free(inflated_data);
}

static inline int png_parse(uint8_t* raw, struct image* img) {
    struct png_parser_state state;
    png_parser_state_init(&state);

    int success = png_read(raw, &state);

    if (success)
        png_load(&state, img);

    free(raw);
    png_parser_state_uninit(&state);

    return success;
}

#endif
