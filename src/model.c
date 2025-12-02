#include "graphics.h"

static struct shader Shader;

static struct model Model;

void draw() {
    window_clear();

    shader_activate(&Shader);

    mat4 projection = camera_projection(DebugCamera, window_aspect_ratio());
    mat4 view = camera_view(DebugCamera);
    mat4 model = rotate_y(10 * window_time());

    shader_set_transform(&Shader, &model, &view, &projection);

    shader_set_vec3(&Shader, "solidColor", vec3_new(1));

    model_draw(&Model);
}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&Shader, "shaders/simple_vs.glsl", "shaders/solid_fs.glsl");

    model_load(&Model, "assets/backpack/backpack.obj");

    debug_camera_init((vec3){0, 0, 8});
    window_register_debug_camera();

    window_set_render_callback(draw);
    window_enable_depth_testing();
    window_set_clear_color(0, 0, 0, 0);

    window_run();
    window_uninit();

    return 0;
}
