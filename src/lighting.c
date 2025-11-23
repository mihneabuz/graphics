#include "graphics.h"

static struct shader ObjectShader;
static struct shader LightShader;

static uint32_t Cube;

float cube_vertices[] = {
    -0.5, -0.5, -0.5, 0.5,  -0.5, -0.5, 0.5,  0.5,  -0.5,  //
    0.5,  0.5,  -0.5, -0.5, 0.5,  -0.5, -0.5, -0.5, -0.5,  //
    -0.5, -0.5, 0.5,  0.5,  -0.5, 0.5,  0.5,  0.5,  0.5,   //
    0.5,  0.5,  0.5,  -0.5, 0.5,  0.5,  -0.5, -0.5, 0.5,   //
    -0.5, 0.5,  0.5,  -0.5, 0.5,  -0.5, -0.5, -0.5, -0.5,  //
    -0.5, -0.5, -0.5, -0.5, -0.5, 0.5,  -0.5, 0.5,  0.5,   //
    0.5,  0.5,  0.5,  0.5,  0.5,  -0.5, 0.5,  -0.5, -0.5,  //
    0.5,  -0.5, -0.5, 0.5,  -0.5, 0.5,  0.5,  0.5,  0.5,   //
    -0.5, -0.5, -0.5, 0.5,  -0.5, -0.5, 0.5,  -0.5, 0.5,   //
    0.5,  -0.5, 0.5,  -0.5, -0.5, 0.5,  -0.5, -0.5, -0.5,  //
    -0.5, 0.5,  -0.5, 0.5,  0.5,  -0.5, 0.5,  0.5,  0.5,   //
    0.5,  0.5,  0.5,  -0.5, 0.5,  0.5,  -0.5, 0.5,  -0.5,  //
};

void init_cube_buffers() {
    glGenVertexArrays(1, &Cube);
    glBindVertexArray(Cube);

    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void draw_cube() {
    glBindVertexArray(Cube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    shader_activate(&LightShader);

    mat4 projection = camera_projection(DebugCamera, window_aspect_ratio());
    shader_set_mat4(&LightShader, "projection", &projection);

    mat4 view = camera_view(DebugCamera);
    shader_set_mat4(&LightShader, "view", &view);

    mat4 model = identity();
    shader_set_mat4(&LightShader, "model", &model);

    draw_cube();
}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&ObjectShader, "shaders/simple_vs.glsl", "shaders/object_fs.glsl");
    shader_init(&LightShader, "shaders/simple_vs.glsl", "shaders/light_fs.glsl");

    init_cube_buffers();

    debug_camera_init((vec3){0, 0, 5});
    window_register_debug_camera();

    window_set_clear_color(0, 0, 0, 1);
    window_set_render_callback(draw);

    window_run();
    window_uninit();

    return 0;
}
