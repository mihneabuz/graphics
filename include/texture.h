#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

#include "gl_loader.h"
#include "image.h"

struct texture {
    uint32_t width, height, channels;
    GLuint id;
};

static inline int texture_load_image(struct texture* tex, const char* path) {
    struct image img;

    int ret = image_load(path, &img);
    if (!ret) {
        tex->width = 0;
        tex->height = 0;
        tex->channels = 0;
        tex->id = 0;
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int format;
    switch (img.channels) {
        case 1:
        case 2:
            image_uninit(&img);
            return 0;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
    };

    glTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0, format, GL_UNSIGNED_BYTE,
                 img.data);

    tex->width = img.width;
    tex->height = img.height;
    tex->channels = img.channels;
    tex->id = texture;

    image_uninit(&img);

    return 1;
}

static inline void texture_bind(struct texture* tex, int index) {
    if (index >= 16)
        return;

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

static inline void texture_unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

static inline void texture_generate_mipmaps(struct texture* tex) {
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glGenerateMipmap(GL_TEXTURE_2D);
}

static inline void texture_uninit(struct texture* tex) {
    glDeleteTextures(1, &tex->id);
}

#endif
