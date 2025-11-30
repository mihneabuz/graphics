#ifndef MESH_H
#define MESH_H

#include "gl_loader.h"
#include "mmath.h"
#include "texture.h"
#include "util.h"

struct vertex {
    vec3 pos;
    vec3 normal;
    vec2 tex_coords;
};

struct texture_pair {
    struct texture diffuse, specular;
};

struct mesh {
    void* data;

    struct vertex* vertices;
    uint32_t vertex_count;

    uint32_t* indices;
    uint32_t index_count;

    struct texture_pair* textures;
    uint32_t texture_count;

    GLuint VAO, VBO, EBO;
};

static inline void mesh_allocate(struct mesh* m,
                                 uint32_t vertex_count,
                                 uint32_t index_count,
                                 uint32_t texture_count) {
    m->vertex_count = vertex_count;
    m->index_count = index_count;
    m->texture_count = texture_count;
    m->data = malloc(vertex_count * sizeof(struct vertex) + index_count * sizeof(uint32_t) +
                     texture_count * sizeof(struct texture));

    if (!m->data)
        panic("mesh_allocate: failed to allocate memory");

    m->vertices = (struct vertex*)m->data;
    m->indices = (uint32_t*)(m->vertices + vertex_count);
    m->textures = (struct texture_pair*)(m->indices + index_count);
}

static inline void mesh_copy_vertices(struct mesh* m, void* data) {
    memcpy(m->vertices, data, m->vertex_count * sizeof(struct vertex));
}

static inline void mesh_copy_indices(struct mesh* m, void* data) {
    memcpy(m->indices, data, m->index_count * sizeof(uint32_t));
}

static inline void mesh_generate(struct mesh* m) {
    glGenVertexArrays(1, &m->VAO);
    glBindVertexArray(m->VAO);

    glGenBuffers(1, &m->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
    glBufferData(GL_ARRAY_BUFFER, m->vertex_count * sizeof(struct vertex), m->vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
                          (void*)offsetof(struct vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
                          (void*)offsetof(struct vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
                          (void*)offsetof(struct vertex, tex_coords));
    glEnableVertexAttribArray(2);

    if (m->index_count > 0) {
        glGenBuffers(1, &m->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->index_count * sizeof(uint32_t), m->indices,
                     GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

static inline void mesh_draw(struct mesh* m) {
    glBindVertexArray(m->VAO);
    if (m->index_count) {
        glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
    }
    glBindVertexArray(0);
}

static inline void mesh_uninit(struct mesh* m) {
    if (m->data)
        free(m->data);

    m->data = nullptr;
    m->vertices = nullptr;
    m->indices = nullptr;
    m->textures = nullptr;
}

#endif
