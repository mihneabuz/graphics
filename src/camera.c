#include "graphics.h"

static struct fly_camera Camera;
static struct shader Shader;
static struct texture Face;
static GLuint VAO;

void init_vertex_data() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cube_stride, cube_positions_offset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, cube_stride, cube_tex_coords_offset);
    glEnableVertexAttribArray(1);
}

void init() {
    shader_init(&Shader, "shaders/transform_vs.glsl", "shaders/transform_fs.glsl");

    texture_load_image(&Face, "assets/awesomeface.png");
    texture_generate_mipmaps(&Face);

    init_vertex_data();

    fly_camera_init(&Camera, (vec3){0, 0, 3});
    Camera.inner.speed = 3;

    glEnable(GL_DEPTH_TEST);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_activate(&Shader);

    texture_bind(&Face, 0);
    shader_set_int(&Shader, "texture0", 0);

    mat4 projection = camera_projection(&Camera.inner, window_aspect_ratio());
    shader_set_mat4(&Shader, "projection", &projection);

    mat4 view = camera_view(&Camera.inner);
    shader_set_mat4(&Shader, "view", &view);

    glBindVertexArray(VAO);

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
        mat4 model = identity();
        mat4_comp(&model, rotate_x(20. * i));
        mat4_comp(&model, rotate_y(6. * i));
        mat4_comp(&model, rotate_z(10. * i));
        mat4_comp(&model, translate(positions[i]));
        shader_set_mat4(&Shader, "model", &model);

        glDrawArrays(GL_TRIANGLES, 0, cube_vertex_count);
    }
}

void forward() {
    camera_move_forward(&Camera.inner, window_delta());
}

void backward() {
    camera_move_backward(&Camera.inner, window_delta());
}

void left() {
    camera_move_left(&Camera.inner, window_delta());
}

void right() {
    camera_move_right(&Camera.inner, window_delta());
}

void up() {
    camera_move_up(&Camera.inner, window_delta());
}

void down() {
    camera_move_down(&Camera.inner, window_delta());
}

void mouse(float x, float y, float xdelta, float ydelta) {
    unused(x) unused(y);
    fly_camera_update(&Camera, xdelta, ydelta);
}

void scroll(float xdelta, float ydelta) {
    unused(xdelta);
    Camera.inner.fov = clamp(Camera.inner.fov - ydelta, 10, 90);
}

int main() {
    if (!window_init(800, 600))
        return 0;

    window_set_clear_color(0.06, 0.08, 0.14, 1.);

    init();

    window_set_render_callback(draw);
    window_set_key_handler(GLFW_KEY_W, forward, 10);
    window_set_key_handler(GLFW_KEY_S, backward, 10);
    window_set_key_handler(GLFW_KEY_A, left, 10);
    window_set_key_handler(GLFW_KEY_D, right, 10);
    window_set_key_handler(GLFW_KEY_LEFT_CONTROL, up, 10);
    window_set_key_handler(GLFW_KEY_LEFT_SHIFT, down, 10);
    window_set_mouse_handler(mouse);
    window_set_scroll_handler(scroll);

    window_run();
    window_uninit();

    return 0;
}
