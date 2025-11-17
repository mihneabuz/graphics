#include "mmath.h"
#include "shader.h"
#include "texture.h"
#include "window.h"

static struct shader Shader;
static struct texture Face;
static GLuint VAO;

void init_vertex_data() {
    float vertices[] = {
        // positions             // texture coords
        0.5,  0.5,  0.0, /**/ 1.0, 1.0,  // top right
        0.5,  -0.5, 0.0, /**/ 1.0, 0.0,  // bottom right
        -0.5, -0.5, 0.0, /**/ 0.0, 0.0,  // bottom left
        -0.5, 0.5,  0.0, /**/ 0.0, 1.0   // top left
    };

    unsigned int elements[] = {0, 1, 2, 0, 3, 2};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void init() {
    shader_init(&Shader, "shaders/transform_vs.glsl", "shaders/transform_fs.glsl");

    texture_load_image(&Face, "assets/awesomeface.png");
    texture_generate_mipmaps(&Face);

    init_vertex_data();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    shader_activate(&Shader);

    texture_bind(&Face, 0);
    shader_set_int(&Shader, "texture1", 0);

    mat4 transform = identity();
    mat4_comp(&transform, translate((vec3){0.5, -0.5, 0.}));
    mat4_comp(&transform, scale((vec3){0.5, 0.5, 0.}));
    mat4_comp(&transform, rotate_z(45));
    shader_set_mat4(&Shader, "transform", &transform);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main() {
    if (!window_init(800, 600))
        return -1;

    window_set_clear_color(0.06, 0.08, 0.14, 1.);

    init();

    window_set_render_callback(draw);
    window_run();
    window_uninit();

    return 0;
}
