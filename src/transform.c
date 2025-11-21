#include "graphics.h"

static struct shader Shader;
static struct texture Face;
static GLuint VAO;

void init_vertex_data() {
    // clang-format off
    float vertices[] = {
        -0.5, -0.5, -0.5,   0.0, 0.0,
         0.5, -0.5, -0.5,   1.0, 0.0,
         0.5,  0.5, -0.5,   1.0, 1.0,
         0.5,  0.5, -0.5,   1.0, 1.0,
        -0.5,  0.5, -0.5,   0.0, 1.0,
        -0.5, -0.5, -0.5,   0.0, 0.0,

        -0.5, -0.5,  0.5,   0.0, 0.0,
         0.5, -0.5,  0.5,   1.0, 0.0,
         0.5,  0.5,  0.5,   1.0, 1.0,
         0.5,  0.5,  0.5,   1.0, 1.0,
        -0.5,  0.5,  0.5,   0.0, 1.0,
        -0.5, -0.5,  0.5,   0.0, 0.0,

        -0.5,  0.5,  0.5,   1.0, 0.0,
        -0.5,  0.5, -0.5,   1.0, 1.0,
        -0.5, -0.5, -0.5,   0.0, 1.0,
        -0.5, -0.5, -0.5,   0.0, 1.0,
        -0.5, -0.5,  0.5,   0.0, 0.0,
        -0.5,  0.5,  0.5,   1.0, 0.0,

         0.5,  0.5,  0.5,   1.0, 0.0,
         0.5,  0.5, -0.5,   1.0, 1.0,
         0.5, -0.5, -0.5,   0.0, 1.0,
         0.5, -0.5, -0.5,   0.0, 1.0,
         0.5, -0.5,  0.5,   0.0, 0.0,
         0.5,  0.5,  0.5,   1.0, 0.0,

        -0.5, -0.5, -0.5,   0.0, 1.0,
         0.5, -0.5, -0.5,   1.0, 1.0,
         0.5, -0.5,  0.5,   1.0, 0.0,
         0.5, -0.5,  0.5,   1.0, 0.0,
        -0.5, -0.5,  0.5,   0.0, 0.0,
        -0.5, -0.5, -0.5,   0.0, 1.0,

        -0.5,  0.5, -0.5,   0.0, 1.0,
         0.5,  0.5, -0.5,   1.0, 1.0,
         0.5,  0.5,  0.5,   1.0, 0.0,
         0.5,  0.5,  0.5,   1.0, 0.0,
        -0.5,  0.5,  0.5,   0.0, 0.0,
        -0.5,  0.5, -0.5,   0.0, 1.0,
    };
    // clang-format on

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
}

void init() {
    shader_init(&Shader, "shaders/transform_vs.glsl", "shaders/transform_fs.glsl");

    texture_load_image(&Face, "assets/awesomeface.png");
    texture_generate_mipmaps(&Face);

    init_vertex_data();

    glEnable(GL_DEPTH_TEST);
}

void draw(float time) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_activate(&Shader);

    texture_bind(&Face, 0);
    shader_set_int(&Shader, "texture0", 0);

    mat4 projection = perspective(60, 8. / 6., 0.1, 100.);
    shader_set_mat4(&Shader, "projection", &projection);

    mat4 view = identity();
    mat4_comp(&view, translate((vec3){0., 0., -3.0}));
    shader_set_mat4(&Shader, "view", &view);

    mat4 model = identity();
    mat4_comp(&model, scale((vec3){1.2, 1.2, 1.2}));
    mat4_comp(&model, rotate_x(45 * time));
    mat4_comp(&model, rotate_y(22.5 * time));
    mat4_comp(&model, rotate_z(22.25 * time));
    shader_set_mat4(&Shader, "model", &model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
