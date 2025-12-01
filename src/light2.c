#include "graphics.h"

static struct shader CrateShader;
static struct shader LightShader;

static struct texture Crate;
static struct texture CrateSpecular;

static struct mesh Cube;

struct dir_light Sun = {
    .dir = {-0.2, -1.0, -0.3},
    .ambient = {0.03, 0.03, 0.01},
    .diffuse = {0.2, 0.2, 0.1},
    .specular = {0.5, 0.5, 0.4},
};

struct point_light Lights[] = {
    {
        .pos = {0.7, 0.2, 2.0},

        .ambient = {0.05, 0.05, 0.05},
        .diffuse = {0.6, 0.6, 0.6},
        .specular = {1.0, 1.0, 1.0},

        .constant = 1,
        .linear = 0.09,
        .quadratic = 0.032,
    },
    {
        .pos = {2.3, -3.3, -4.0},

        .ambient = {0.05, 0.05, 0.05},
        .diffuse = {0.6, 0.6, 0.6},
        .specular = {1.0, 1.0, 1.0},

        .constant = 1,
        .linear = 0.09,
        .quadratic = 0.032,
    },
    {
        .pos = {-4.0, 2.0, -12.0},

        .ambient = {0.05, 0.05, 0.05},
        .diffuse = {0.6, 0.6, 0.6},
        .specular = {1.0, 1.0, 1.0},

        .constant = 1,
        .linear = 0.09,
        .quadratic = 0.032,
    },
    {
        .pos = {0.0, 0.0, -3.0},

        .ambient = {0.05, 0.05, 0.05},
        .diffuse = {0.6, 0.6, 0.6},
        .specular = {1.0, 1.0, 1.0},

        .constant = 1,
        .linear = 0.09,
        .quadratic = 0.032,
    },
};

struct spot_light Flashlight = {
    .ambient = {0, 0, 0},
    .diffuse = {1, 1, 1},
    .specular = {1, 1, 1},

    .constant = 1,
    .linear = 0.09,
    .quadratic = 0.032,

    .cutoff = 0,
    .outerCutoff = 0,
};

void init_cube_mesh() {
    mesh_allocate(&Cube, cube_vertex_count, 0);
    mesh_copy_vertices(&Cube, cube_vertices);
    mesh_generate(&Cube);
}

void draw_crates() {
    shader_activate(&CrateShader);

    mat4 projection = camera_projection(DebugCamera, window_aspect_ratio());
    shader_set_mat4(&CrateShader, "projection", &projection);

    mat4 view = camera_view(DebugCamera);
    shader_set_mat4(&CrateShader, "view", &view);

    shader_set_directional_light(&CrateShader, "dirLight", &Sun);
    shader_set_point_light(&CrateShader, "pointLights[0]", &Lights[0]);
    shader_set_point_light(&CrateShader, "pointLights[1]", &Lights[1]);
    shader_set_point_light(&CrateShader, "pointLights[2]", &Lights[2]);
    shader_set_point_light(&CrateShader, "pointLights[3]", &Lights[3]);

    Flashlight.cutoff = cos(radians(12.5));
    Flashlight.outerCutoff = cos(radians(15));
    Flashlight.pos = camera_pos(DebugCamera);
    Flashlight.dir = camera_front(DebugCamera);
    shader_set_spot_light(&CrateShader, "spotLight", &Flashlight);

    texture_bind(&Crate, 0);
    texture_bind(&CrateSpecular, 1);
    struct material_map material = {
        .diffuse_sampler = 0,
        .specular_sampler = 1,
        .shininess = 32,
    };
    shader_set_material_map(&CrateShader, "material", &material);

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

    for (int i = 0; i < 10; i++) {
        mat4 model = translate(positions[i]);
        shader_set_mat4(&CrateShader, "model", &model);

        mesh_draw(&Cube);
    }
}

void draw_lights() {
    shader_activate(&LightShader);

    mat4 projection = camera_projection(DebugCamera, window_aspect_ratio());
    shader_set_mat4(&CrateShader, "projection", &projection);

    mat4 view = camera_view(DebugCamera);
    shader_set_mat4(&CrateShader, "view", &view);

    for (int i = 0; i < 4; i++) {
        mat4 model = identity();
        mat4_comp(&model, scale(vec3_new(0.2)));
        mat4_comp(&model, translate(Lights[i].pos));
        shader_set_mat4(&LightShader, "model", &model);

        shader_set_vec3(&LightShader, "solidColor", Lights[i].specular);

        mesh_draw(&Cube);
    }
}

void draw() {
    window_clear();

    draw_crates();
    draw_lights();
}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&CrateShader, "shaders/simple_vs.glsl", "shaders/light2_fs.glsl");
    shader_init(&LightShader, "shaders/simple_vs.glsl", "shaders/solid_fs.glsl");

    texture_load_image(&Crate, "assets/crate.png");
    texture_load_image(&CrateSpecular, "assets/crate_specular.png");

    init_cube_mesh();

    debug_camera_init((vec3){0, 0.2, 5});
    window_register_debug_camera();

    window_enable_depth_testing();
    window_set_clear_color(0, 0, 0, 1);
    window_set_render_callback(draw);

    window_run();
    window_uninit();

    return 0;
}
