#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "vector.h"

struct model {
    struct vector meshes;
};

static inline struct mesh _model_process_mesh(struct aiMesh* mesh) {
    uint32_t vertex_count = mesh->mNumVertices;

    uint32_t index_count = 0;
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        index_count += mesh->mFaces[i].mNumIndices;

    struct mesh out;
    mesh_allocate(&out, vertex_count, index_count);

    for (uint32_t i = 0; i < vertex_count; i++) {
        out.vertices[i].pos.x = mesh->mVertices[i].x;
        out.vertices[i].pos.y = mesh->mVertices[i].y;
        out.vertices[i].pos.z = mesh->mVertices[i].z;

        out.vertices[i].normal.x = mesh->mNormals[i].x;
        out.vertices[i].normal.y = mesh->mNormals[i].y;
        out.vertices[i].normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            out.vertices[i].tex_coords.x = mesh->mTextureCoords[0][i].x;
            out.vertices[i].tex_coords.y = mesh->mTextureCoords[0][i].y;
        } else {
            out.vertices[i].tex_coords = (vec2){0, 0};
        }
    }

    uint32_t k = 0;
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            out.indices[k++] = face.mIndices[j];
    }

    mesh_generate(&out);
    return out;
}

static inline void _model_process_node(struct model* mod,
                                       struct aiNode* node,
                                       const struct aiScene* scene) {
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        struct mesh m = _model_process_mesh(mesh);
        vec_push(&mod->meshes, &m);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
        _model_process_node(mod, node->mChildren[i], scene);
}

static inline void model_load(struct model* mod, const char* path) {
    vec_init(&mod->meshes, sizeof(struct mesh));

    const struct aiScene* scene =
        aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        panic("model_load: failed to load model\n%s", aiGetErrorString());

    _model_process_node(mod, scene->mRootNode, scene);
}

static inline void model_draw(struct model* mod) {
    for (struct mesh* p = vec_iter_start(&mod->meshes); p != vec_iter_end(&mod->meshes);
         vec_iter_advance(&mod->meshes, (void*)&p)) {
        mesh_draw(p);
    }
    vec_for_each(&mod->meshes, (void (*)(void*))mesh_draw);
}

static inline void model_uninit(struct model* mod) {
    vec_for_each(&mod->meshes, (void (*)(void*))mesh_uninit);
    vec_uninit(&mod->meshes);
}

#endif
