#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "mstring.h"
#include "shader.h"
#include "texture.h"
#include "util.h"
#include "vector.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

struct model {
    struct string path;
    struct vector meshes;
    struct vector materials;
};

struct model_mesh {
    struct mesh mesh;
    uint32_t material_id;
};

struct model_material {
    struct texture diffuse, specular;
    float shininess;
};

static inline void _model_process_materials(struct model* mod, const struct aiScene* scene) {
    struct string path;
    string_clone(&mod->path, &path);
    string_pop_until(&path, '/');

    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        struct aiMaterial* mat = scene->mMaterials[i];
        struct model_material out = {0};
        struct aiString tex;

        if (aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE) > 0) {
            aiGetMaterialString(mat, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), &tex);
            string_append(&path, (const char*)&tex.data, tex.length);
            texture_load_image(&out.diffuse, string_ptr(&path));
            string_pop(&path, tex.length);
        } else {
            texture_create_fallback(&out.diffuse, (vec4){0, 0.2, 0.6, 1});
        };

        if (aiGetMaterialTextureCount(mat, aiTextureType_SPECULAR) > 0) {
            aiGetMaterialString(mat, AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), &tex);
            string_append(&path, (const char*)&tex.data, tex.length);
            texture_load_image(&out.specular, string_ptr(&path));
            string_pop(&path, tex.length);
        } else {
            texture_create_fallback(&out.specular, (vec4){1, 1, 1, 1});
        };

        if (!aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &out.shininess))
            out.shininess = 16;

        vec_push(&mod->materials, &out);
    }

    string_uninit(&path);
}

static inline struct model_mesh _model_process_mesh(struct aiMesh* mesh) {
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

    return (struct model_mesh){
        .mesh = out,
        .material_id = mesh->mMaterialIndex,
    };
}

static inline void _model_process_node(struct model* mod,
                                       struct aiNode* node,
                                       const struct aiScene* scene) {
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        struct model_mesh m = _model_process_mesh(mesh);
        vec_push(&mod->meshes, &m);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
        _model_process_node(mod, node->mChildren[i], scene);
}

static inline void model_load(struct model* mod, const char* path) {
    string_init(&mod->path);
    vec_init(&mod->meshes, sizeof(struct model_mesh));
    vec_init(&mod->materials, sizeof(struct model_material));

    const struct aiScene* scene =
        aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        panic("model_load: failed to load model\n%s", aiGetErrorString());

    string_append(&mod->path, path, strlen(path));

    _model_process_node(mod, scene->mRootNode, scene);
    _model_process_materials(mod, scene);
}

static inline void model_draw(struct model* mod, struct shader* shader) {
    shader_set_int(shader, "material.diffuse", 0);
    shader_set_int(shader, "material.specular", 1);

    uint32_t last_material_id = -1;
    for (struct model_mesh* p = vec_iter_start(&mod->meshes); p != vec_iter_end(&mod->meshes);
         vec_iter_advance(&mod->meshes, (void*)&p)) {
        if (p->material_id != last_material_id) {
            struct model_material* material = vec_item(&mod->materials, p->material_id);
            texture_bind(&material->diffuse, 0);
            texture_bind(&material->specular, 1);
            shader_set_float(shader, "material.shininess", material->shininess);
        }

        mesh_draw(&p->mesh);

        last_material_id = p->material_id;
    }
}

static inline void model_uninit(struct model* mod) {
    for (struct model_mesh* p = vec_iter_start(&mod->meshes); p != vec_iter_end(&mod->meshes);
         vec_iter_advance(&mod->meshes, (void*)&p))
        mesh_uninit(&p->mesh);

    for (struct model_material* p = vec_iter_start(&mod->materials);
         p != vec_iter_end(&mod->materials); vec_iter_advance(&mod->materials, (void*)&p)) {
        texture_uninit(&p->diffuse);
        texture_uninit(&p->specular);
    }

    vec_uninit(&mod->materials);
    vec_uninit(&mod->meshes);
    string_uninit(&mod->path);
}

#endif
