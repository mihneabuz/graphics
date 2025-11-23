#ifndef SHADER_H
#define SHADER_H

#include "gl_loader.h"
#include "mmath.h"
#include "util.h"

struct shader {
    GLuint program;
};

static inline GLuint compile_shader(const char* path, GLuint type) {
    const char* source = read_file(path);
    if (!source)
        panic("compile_shader:\nfailed to read file %s\n", path);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    free((void*)source);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        panic("compile_shader:\nfailed to compile %s\n%s", path, infoLog);
    }

    return shader;
}

static inline GLuint link_shaders(GLuint vertex, GLuint fragment) {
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        panic("program linking failed:\n%s\n", infoLog);
    }

    return program;
}

static inline void shader_init(struct shader* shader, const char* vertex, const char* fragment) {
    shader->program = 0;

    GLuint vertex_shader = compile_shader(vertex, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment, GL_FRAGMENT_SHADER);
    GLuint program = link_shaders(vertex_shader, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    shader->program = program;
}

static inline void shader_activate(struct shader* shader) {
    glUseProgram(shader->program);
}

static inline void shader_deactivate() {
    glUseProgram(0);
}

static inline void shader_set_int(struct shader* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->program, name), value);
}

static inline void shader_set_float(struct shader* shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->program, name), value);
}

static inline void shader_set_vec3(struct shader* shader, const char* name, vec3 vec) {
    glUniform3f(glGetUniformLocation(shader->program, name), vec.x, vec.y, vec.z);
}

static inline void shader_set_mat3(struct shader* shader, const char* name, mat3* mat) {
    glUniformMatrix3fv(glGetUniformLocation(shader->program, name), 1, GL_FALSE, (float*)mat);
}

static inline void shader_set_mat4(struct shader* shader, const char* name, mat4* mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader->program, name), 1, GL_FALSE, (float*)mat);
}

static inline void shader_uninit(struct shader* shader) {
    glDeleteProgram(shader->program);
}

#endif
