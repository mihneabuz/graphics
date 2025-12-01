#include "graphics.h"

static struct shader ObjectShader;
static struct shader LightShader;

static struct texture Crate;
static struct texture CrateSpecular;
static struct texture Checkered;
static struct texture Black;

static struct mesh Floor;
static struct mesh Cube;

static struct point_light Light = {
    .pos = {0.0, 3.0, 1.0},

    .ambient = {0.1, 0.1, 0.1},
    .diffuse = {0.6, 0.6, 0.6},
    .specular = {1, 1, 1},

    .constant = 1.0,
    .linear = 0.09,
    .quadratic = 0.032,
};

static struct material SimpleCube = {
    .ambient = {1.0, 0.5, 0.32},
    .diffuse = {1.0, 0.5, 0.32},
    .specular = {0.5, 0.5, 0.5},
    .shininess = 16,
};

static struct material_map TexturedCube = {
    .shininess = 32,
};

static struct material_map FloorMat = {
    .shininess = 1,
};

static mat4 View;
static mat4 Projection;

void init_cube_mesh() {
    mesh_allocate(&Cube, cube_vertex_count, 0);
    mesh_copy_vertices(&Cube, cube_vertices);
    mesh_generate(&Cube);
}

void init_floor_mesh() {
    const struct vertex floor_vertices[] = {
        {{-0.5, 0, 0.5}, {0, 1, 0}, {0, 0}},   //
        {{0.5, 0, 0.5}, {0, 1, 0}, {0, 0}},    //
        {{-0.5, 0, -0.5}, {0, 1, 0}, {0, 0}},  //
        {{0.5, 0, -0.5}, {0, 1, 0}, {0, 0}},   //
    };

    const uint32_t floor_indices[] = {
        0, 1, 2,  //
        1, 2, 3,  //
    };

    mesh_allocate(&Floor, 4, 6);
    mesh_copy_vertices(&Floor, (char*)floor_vertices);
    mesh_copy_indices(&Floor, (char*)floor_indices);
    mesh_generate(&Floor);
}

void draw_floor() {
    shader_activate(&ObjectShader);

    mat4 model = identity();
    mat4_comp(&model, scale((vec3){1000, 1, 1000}));
    mat4_comp(&model, translate((vec3){0, 1, 0}));
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    texture_bind(&Checkered, 0);
    texture_bind(&Black, 1);
    FloorMat.diffuse_sampler = 0;
    FloorMat.specular_sampler = 1;

    shader_set_vec3(&ObjectShader, "viewPos", camera_pos(DebugCamera));
    shader_set_point_light(&ObjectShader, "light", &Light);
    shader_set_material_map(&ObjectShader, "materialMap", &FloorMat);
    shader_set_int(&ObjectShader, "useGeneratedCoords", GL_TRUE);
    shader_set_int(&ObjectShader, "useTexture", GL_TRUE);

    mesh_draw(&Floor);
}

void draw_light() {
    shader_activate(&LightShader);

    mat4 model = identity();
    mat4_comp(&model, scale((vec3){0.2, 0.2, 0.2}));
    mat4_comp(&model, translate(Light.pos));
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    shader_set_vec3(&LightShader, "solidColor", vec3_new(1));

    mesh_draw(&Cube);
}

void draw_simple_object() {
    shader_activate(&ObjectShader);

    mat4 model = translate((vec3){-1.5, 1.5, 0});
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    shader_set_vec3(&ObjectShader, "viewPos", camera_pos(DebugCamera));
    shader_set_point_light(&ObjectShader, "light", &Light);
    shader_set_material(&ObjectShader, "material", &SimpleCube);
    shader_set_int(&ObjectShader, "useGeneratedCoords", GL_FALSE);
    shader_set_int(&ObjectShader, "useTexture", GL_FALSE);

    mesh_draw(&Cube);
}

void draw_textured_object() {
    shader_activate(&ObjectShader);

    mat4 model = translate((vec3){1.5, 1.5, 0});
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    texture_bind(&Crate, 2);
    texture_bind(&CrateSpecular, 3);
    TexturedCube.diffuse_sampler = 2;
    TexturedCube.specular_sampler = 3;

    shader_set_vec3(&ObjectShader, "viewPos", camera_pos(DebugCamera));
    shader_set_point_light(&ObjectShader, "light", &Light);
    shader_set_material_map(&ObjectShader, "materialMap", &TexturedCube);
    shader_set_int(&ObjectShader, "useGeneratedCoords", GL_FALSE);
    shader_set_int(&ObjectShader, "useTexture", GL_TRUE);

    mesh_draw(&Cube);
}

void draw() {
    window_clear();

    Projection = camera_projection(DebugCamera, window_aspect_ratio());
    View = camera_view(DebugCamera);

    draw_floor();
    draw_light();

    draw_simple_object();
    draw_textured_object();
}

void light_up() {
    Light.pos.y += 0.1;
}

void light_down() {
    Light.pos.y -= 0.1;
}

void light_left() {
    Light.pos.x -= 0.1;
}

void light_right() {
    Light.pos.x += 0.1;
}

void light_forward() {
    Light.pos.z -= 0.1;
}

void light_backward() {
    Light.pos.z += 0.1;
}

int main() {
    if (!window_init(800, 600))
        return 1;

    shader_init(&ObjectShader, "shaders/simple_vs.glsl", "shaders/light1_fs.glsl");
    shader_init(&LightShader, "shaders/simple_vs.glsl", "shaders/solid_fs.glsl");

    texture_load_image(&Crate, "assets/crate.png");
    texture_load_image(&CrateSpecular, "assets/crate_specular.png");
    texture_load_image(&Checkered, "assets/checkered.png");
    texture_create_fallback(&Black, (vec4){0, 0, 0, 0});

    init_cube_mesh();
    init_floor_mesh();

    debug_camera_init((vec3){0, 3, 5});
    window_register_debug_camera();

    window_enable_depth_testing();
    window_set_clear_color(0, 0, 0, 1);
    window_set_render_callback(draw);

    window_set_key_handler(GLFW_KEY_H, light_left, 10);
    window_set_key_handler(GLFW_KEY_J, light_down, 10);
    window_set_key_handler(GLFW_KEY_K, light_up, 10);
    window_set_key_handler(GLFW_KEY_L, light_right, 10);
    window_set_key_handler(GLFW_KEY_Y, light_backward, 10);
    window_set_key_handler(GLFW_KEY_U, light_forward, 10);

    window_run();
    window_uninit();

    return 0;
}
