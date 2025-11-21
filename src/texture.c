#include "graphics.h"

static struct shader Shader;
static struct texture Wall;
static struct texture Face;
static float blend = 0.6;
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
    shader_init(&Shader, "shaders/texture_vs.glsl", "shaders/texture_fs.glsl");

    texture_load_image(&Wall, "assets/wall.png");
    texture_generate_mipmaps(&Wall);

    texture_load_image(&Face, "assets/awesomeface.png");
    texture_generate_mipmaps(&Face);

    init_vertex_data();
}

void draw(float time) {
    glClear(GL_COLOR_BUFFER_BIT);

    shader_activate(&Shader);

    texture_bind(&Wall, 0);
    shader_set_int(&Shader, "texture0", 0);

    texture_bind(&Face, 1);
    shader_set_int(&Shader, "texture1", 1);

    mat4 transform = rotate_y(45 * time);
    shader_set_mat4(&Shader, "transform", &transform);

    shader_set_float(&Shader, "blend", blend);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void increase_blend() {
    blend += 0.01;
    if (blend > 1.)
        blend = 1.;
}

void decrease_blend() {
    blend -= 0.01;
    if (blend < 0.)
        blend = 0.;
}

int main() {
    if (!window_init(800, 600))
        return -1;

    window_set_clear_color(0.06, 0.08, 0.14, 1.);

    init();

    window_set_render_callback(draw);
    window_set_key_handler(GLFW_KEY_UP, increase_blend, 10);
    window_set_key_handler(GLFW_KEY_DOWN, decrease_blend, 10);
    window_run();
    window_uninit();

    return 0;
}
