#include "glad/glad.h"

#include <math.h>
#include <stdio.h>

#include "shader.h"
#include "window.h"

static struct shader Shader;

static GLuint TriangleVAO;

void init_vertex_data() {
    float vertices[] = {
        // positions             // colors
        0.5f,  -0.5f, 0.0f, /**/ 1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, /**/ 0.0f, 1.0f, 0.0f,  // bottom left
        0.0f,  0.5f,  0.0f, /**/ 0.0f, 0.0f, 1.0f,  // top
    };

    glGenVertexArrays(1, &TriangleVAO);
    glBindVertexArray(TriangleVAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    shader_activate(&Shader);
    float xOffset = sin(glfwGetTime()) / 2.4;
    float yOffset = cos(glfwGetTime()) / 2.4;
    shader_set_float(&Shader, "xOffset", xOffset);
    shader_set_float(&Shader, "yOffset", yOffset);

    glBindVertexArray(TriangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main() {
    if (!window_init(800, 600))
        return -1;

    window_set_clear_color(0.06f, 0.08f, 0.14f, 1.0f);

    shader_init(&Shader, "shaders/triangle_vs.glsl", "shaders/triangle_fs.glsl");
    init_vertex_data();

    window_set_render_callback(draw);
    window_run();

    window_uninit();

    return 0;
}
