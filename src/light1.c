#include "graphics.h"
#include "shader.h"
#include "texture.h"

static struct shader ObjectShader;
static struct shader LightShader;

static struct texture Crate;
static struct texture CrateSpecular;
static struct texture Checkered;
static struct texture Black;

static uint32_t Floor;
static uint32_t Cube;

static struct light Light = {
    .pos = {0.0, 2.0, 1.0},
    .ambient = {0.1, 0.1, 0.1},
    .diffuse = {0.6, 0.6, 0.6},
    .specular = {1, 1, 1},
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

void init_cube_buffers() {
    // clang-format off
    float cube_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    // clang-format on

    glGenVertexArrays(1, &Cube);
    glBindVertexArray(Cube);

    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void init_floor_buffers() {
    const float floor_vertices[] = {
        -0.5, 0, 0.5,   //
        0.5,  0, 0.5,   //
        -0.5, 0, -0.5,  //
        0.5,  0, -0.5,  //
    };

    const uint32_t floor_indices[] = {
        0, 1, 2,  //
        1, 2, 3,  //
    };

    glGenVertexArrays(1, &Floor);
    glBindVertexArray(Floor);

    uint32_t buffers[2] = {0, 0};
    glGenBuffers(2, buffers);

    uint32_t* VBO = &buffers[0];
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

    uint32_t* EBO = &buffers[1];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttrib3f(1, 0, 1.0, 0);
    glDisableVertexAttribArray(1);

    glDisableVertexAttribArray(2);
}

void draw_floor() {
    shader_activate(&ObjectShader);

    mat4 model = scale((vec3){1000, 0, 1000});
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    texture_bind(&Checkered, 0);
    texture_bind(&Black, 1);
    FloorMat.diffuse_sampler = 0;
    FloorMat.specular_sampler = 1;

    shader_set_vec3(&ObjectShader, "viewPos", camera_pos(DebugCamera));
    shader_set_light(&ObjectShader, "light", &Light);
    shader_set_material_map(&ObjectShader, "materialMap", &FloorMat);
    shader_set_int(&ObjectShader, "useGeneratedCoords", GL_TRUE);
    shader_set_int(&ObjectShader, "useTexture", GL_TRUE);

    glBindVertexArray(Floor);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void draw_cube() {
    glBindVertexArray(Cube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void draw_light() {
    shader_activate(&LightShader);

    mat4 model = identity();
    mat4_comp(&model, scale((vec3){0.2, 0.2, 0.2}));
    mat4_comp(&model, translate(Light.pos));
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    draw_cube();
}

void draw_simple_object() {
    shader_activate(&ObjectShader);

    mat4 model = translate((vec3){-1.5, 0.5, 0});
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    shader_set_vec3(&ObjectShader, "viewPos", camera_pos(DebugCamera));
    shader_set_light(&ObjectShader, "light", &Light);
    shader_set_material(&ObjectShader, "material", &SimpleCube);
    shader_set_int(&ObjectShader, "useGeneratedCoords", GL_FALSE);
    shader_set_int(&ObjectShader, "useTexture", GL_FALSE);

    draw_cube();
}

void draw_textured_object() {
    shader_activate(&ObjectShader);

    mat4 model = translate((vec3){1.5, 0.5, 0});
    shader_set_transform(&ObjectShader, &model, &View, &Projection);

    texture_bind(&Crate, 2);
    texture_bind(&CrateSpecular, 3);
    TexturedCube.diffuse_sampler = 2;
    TexturedCube.specular_sampler = 3;

    shader_set_vec3(&ObjectShader, "viewPos", camera_pos(DebugCamera));
    shader_set_light(&ObjectShader, "light", &Light);
    shader_set_material_map(&ObjectShader, "materialMap", &TexturedCube);
    shader_set_int(&ObjectShader, "useGeneratedCoords", GL_FALSE);
    shader_set_int(&ObjectShader, "useTexture", GL_TRUE);

    draw_cube();
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
    shader_init(&LightShader, "shaders/simple_vs.glsl", "shaders/white_fs.glsl");

    texture_load_image(&Checkered, "assets/checkered.png");
    texture_generate_mipmaps(&Checkered);

    texture_create_fallback(&Black, (vec4){0, 0, 0, 0});
    texture_generate_mipmaps(&Black);

    texture_load_image(&Crate, "assets/crate.png");
    texture_generate_mipmaps(&Crate);

    texture_load_image(&CrateSpecular, "assets/crate_specular.png");
    texture_generate_mipmaps(&CrateSpecular);

    init_cube_buffers();
    init_floor_buffers();

    debug_camera_init((vec3){0, 2, 5});
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
