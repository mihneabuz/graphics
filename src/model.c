#include "graphics.h"

static struct shader Shader;

static struct model Model;

void draw() {}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&Shader, "shaders/simple_vs.glsl", "shaders/solid_fs.glsl");

    model_load(&Model, "assets/nope");

    debug_camera_init((vec3){0, 3, 5});
    window_register_debug_camera();

    window_set_render_callback(draw);
    window_enable_depth_testing();
    window_set_clear_color(0, 0, 0, 0);

    window_run();
    window_uninit();

    return 0;
}
