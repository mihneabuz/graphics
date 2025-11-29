#include "graphics.h"
#include "shader.h"
#include "texture.h"

static struct shader Shader;

static struct texture Crate;
static struct texture CrateSpecular;

static uint32_t Cube;

static mat4 View;
static mat4 Projection;

void init_cube_buffers() {
    glGenVertexArrays(1, &Cube);
    glBindVertexArray(Cube);

    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cube_stride, cube_positions_offset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cube_stride, cube_normals_offset);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, cube_stride, cube_tex_coords_offset);
    glEnableVertexAttribArray(2);
}

void draw_cube() {
    glBindVertexArray(Cube);
    glDrawArrays(GL_TRIANGLES, 0, cube_vertex_count);
}

void draw() {
    window_clear();

    shader_activate(&Shader);

    Projection = camera_projection(DebugCamera, window_aspect_ratio());
    View = camera_view(DebugCamera);

    vec3 positions[10] = {
        {0.0, 0.0, 0.0},      //
        {2.0, 5.0, -15.0},    //
        {-1.5, -2.2, -2.5},   //
        {-3.8, -2.0, -12.3},  //
        {2.4, -0.4, -3.5},    //
        {-1.7, 3.0, -7.5},    //
        {1.3, -2.0, -2.5},    //
        {1.5, 2.0, -2.5},     //
        {1.5, 0.2, -1.5},     //
        {-1.3, 1.0, -1.5},    //
    };

    texture_bind(&Crate, 0);
    texture_bind(&CrateSpecular, 1);
    struct material_map material = {
        .diffuse_sampler = 0,
        .specular_sampler = 1,
        .shininess = 32,
    };
    shader_set_material_map(&Shader, "material", &material);

    for (int i = 0; i < 10; i++) {
        mat4 model = identity();
        mat4_comp(&model, translate(positions[i]));
        shader_set_transform(&Shader, &model, &View, &Projection);

        draw_cube();
    }
}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&Shader, "shaders/simple_vs.glsl", "shaders/light2_fs.glsl");

    texture_load_image(&Crate, "assets/crate.png");
    texture_generate_mipmaps(&Crate);

    texture_load_image(&CrateSpecular, "assets/crate_specular.png");
    texture_generate_mipmaps(&CrateSpecular);

    init_cube_buffers();

    debug_camera_init((vec3){0, 0.2, 5});
    window_register_debug_camera();

    window_enable_depth_testing();
    window_set_clear_color(0, 0, 0, 1);
    window_set_render_callback(draw);

    window_run();
    window_uninit();

    return 0;
}
