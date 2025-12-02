#include "graphics.h"

static struct shader Shader;

static struct model Model;

static struct point_light Light = {
    .pos = {0.0, 3.0, 2.0},

    .ambient = {0.1, 0.1, 0.1},
    .diffuse = {0.5, 0.5, 0.5},
    .specular = {1, 1, 1},

    .constant = 1.0,
    .linear = 0.14,
    .quadratic = 0.07,
};

void draw() {
    window_clear();

    shader_activate(&Shader);

    shader_set_point_light(&Shader, "light", &Light);
    shader_set_vec3(&Shader, "viewPos", camera_pos(DebugCamera));

    mat4 projection = camera_projection(DebugCamera, window_aspect_ratio());
    mat4 view = camera_view(DebugCamera);
    mat4 model = rotate_y(10 * window_time());
    shader_set_transform(&Shader, &model, &view, &projection);

    model_draw(&Model, &Shader);
}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&Shader, "shaders/simple_vs.glsl", "shaders/model_fs.glsl");

    model_load(&Model, "assets/backpack/backpack.obj");

    debug_camera_init((vec3){0, 0, 8});
    window_register_debug_camera();

    window_set_render_callback(draw);
    window_enable_depth_testing();
    window_set_clear_color(0, 0, 0, 0);

    window_run();
    window_uninit();

    model_uninit(&Model);
    shader_uninit(&Shader);

    return 0;
}
