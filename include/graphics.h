#include "window.h"

#include "camera.h"
#include "mmath.h"
#include "shader.h"
#include "texture.h"

struct fly_camera DebugFlyCamera;
struct camera* DebugCamera = &DebugFlyCamera.inner;

void debug_camera_init(vec3 pos) {
    fly_camera_init(&DebugFlyCamera, pos);
    DebugFlyCamera.sensitivity = 0.3;
    DebugFlyCamera.inner.speed = 5;
    camera_set_y_bounds(DebugCamera, 0.2, INFINITY);
}

void debug_camera_forward() {
    camera_move_forward(DebugCamera, window_delta());
}

void debug_camera_backward() {
    camera_move_backward(DebugCamera, window_delta());
}

void debug_camera_left() {
    camera_move_left(DebugCamera, window_delta());
}

void debug_camera_right() {
    camera_move_right(DebugCamera, window_delta());
}

void debug_camera_up() {
    camera_move_up(DebugCamera, window_delta());
}

void debug_camera_down() {
    camera_move_down(DebugCamera, window_delta());
}

void debug_camera_mouse(float x, float y, float xdelta, float ydelta) {
    unused(x) unused(y);
    fly_camera_update(&DebugFlyCamera, xdelta, ydelta);
}

void debug_camera_scroll(float xdelta, float ydelta) {
    unused(xdelta);
    DebugCamera->fov = clamp(DebugCamera->fov - ydelta, 10, 90);
}

void window_register_debug_camera() {
    window_set_key_handler(GLFW_KEY_W, debug_camera_forward, 10);
    window_set_key_handler(GLFW_KEY_S, debug_camera_backward, 10);
    window_set_key_handler(GLFW_KEY_A, debug_camera_left, 10);
    window_set_key_handler(GLFW_KEY_D, debug_camera_right, 10);
    window_set_key_handler(GLFW_KEY_LEFT_CONTROL, debug_camera_up, 10);
    window_set_key_handler(GLFW_KEY_LEFT_SHIFT, debug_camera_down, 10);
    window_set_mouse_handler(debug_camera_mouse);
    window_set_scroll_handler(debug_camera_scroll);
}
