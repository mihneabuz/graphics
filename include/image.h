#ifndef IMAGE_H
#define IMAGE_H

#include "util.h"

#include "image/png.h"
#include "image/struct.h"

struct image_loader {
    uint8_t* sig;
    uint8_t sig_len;
    int (*parser)(uint8_t* raw, struct image* img);
};

static inline int image_load(const char* path, struct image* img) {
    img->width = 0;
    img->height = 0;
    img->channels = 0;
    img->data = nullptr;

    uint8_t* raw = (uint8_t*)read_file(path);
    if (!raw)
        return 0;

    struct image_loader loaders[] = {
        {png_sig, sizeof(png_sig), png_parse},
    };

    for (uint32_t i = 0; i < sizeof(loaders) / sizeof(struct image_loader); i++) {
        struct image_loader* l = &loaders[i];
        if (memcmp(raw, l->sig, l->sig_len) == 0)
            return l->parser(raw, img);
    }

    free(raw);
    return 1;
}

static inline int image_write_ppm(struct image* img, FILE* file) {
    fwrite("P3\n", 3, 1, file);
    fprintf(file, "%d %d\n255\n", img->width, img->height);

    for (uint32_t i = 0; i < img->height; i++) {
        for (uint32_t j = 0; j < img->width; j++) {
            uint8_t* pixel = &img->data[(i * img->width + j) * img->channels];
            fprintf(file, " %d %d %d ", pixel[0], pixel[1], pixel[2]);
        }
        fwrite("\n", 1, 1, file);
    }

    fclose(file);
    return 1;
}

static inline int image_write(struct image* img, const char* path, const char* format) {
    if (!img->data)
        return 0;

    FILE* file = fopen(path, "w+");
    if (!file)
        return 0;

    if (strncmp(format, "ppm", 3) == 0)
        return image_write_ppm(img, file);

    fclose(file);
    return 1;
}

static inline void image_uninit(struct image* img) {
    if (img->data) {
        free(img->data);
        img->data = nullptr;
    }
}

#endif
