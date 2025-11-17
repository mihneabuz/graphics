#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl_loader.h"

#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct texture {
    uint32_t width, height, channels;
    GLuint id;
};

static inline int texture_load_image(struct texture* tex, const char* path) {
    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
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

    int format = channels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    tex->width = width;
    tex->height = height;
    tex->channels = channels;
    tex->id = texture;

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
