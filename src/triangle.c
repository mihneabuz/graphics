#include "glad/glad.h"

#include <math.h>

#include "shader.h"
#include "window.h"

static struct shader Shader;
static GLuint TriangleVAO;

static float speed = 1.0;
static float last_draw = 0.0;
static float accumulator = 0.0;

void init_vertex_data() {
    float vertices[] = {
        // positions          // colors
        0.5,  -0.5, 0.0, /**/ 1.0, 0.0, 0.0,  // bottom right
        -0.5, -0.5, 0.0, /**/ 0.0, 1.0, 0.0,  // bottom left
        0.0,  0.5,  0.0, /**/ 0.0, 0.0, 1.0,  // top
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

    float time = glfwGetTime();
    float elapsed = last_draw - time;
    accumulator += elapsed * speed;
    last_draw = time;

    shader_activate(&Shader);
    float xOffset = sin(accumulator) / 2.4;
    float yOffset = cos(accumulator) / 2.4;
    shader_set_float(&Shader, "xOffset", xOffset);
    shader_set_float(&Shader, "yOffset", yOffset);

    glBindVertexArray(TriangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void increase_speed() {
    speed += 0.02;
}

void decrease_speed() {
    speed -= 0.02;
    if (speed < 0.)
        speed = 0.;
}

int main() {
    if (!window_init(800, 600))
        return -1;

    window_set_clear_color(0.06, 0.08, 0.14, 1.);

    shader_init(&Shader, "shaders/triangle_vs.glsl", "shaders/triangle_fs.glsl");
    init_vertex_data();

    window_set_render_callback(draw);
    window_set_key_handler(GLFW_KEY_UP, increase_speed, 10);
    window_set_key_handler(GLFW_KEY_DOWN, decrease_speed, 10);
    window_run();

    window_uninit();

    return 0;
}
