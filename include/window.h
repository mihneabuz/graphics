#include <stdio.h>

#define GL_LOADER
#include "gl_loader.h"

#include "util.h"
#include "vector.h"

#include <GLFW/glfw3.h>

struct key_handler {
    callback handler;
    int key;
    double debounce;
    double last_press;
};

typedef void (*mouse_callback)(float x, float y, float xdelta, float ydelta);

struct mouse {
    mouse_callback handler;
    float lastx;
    float lasty;
    uint8_t init;
};

struct window {
    float time;
    float delta;
    uint32_t width;
    uint32_t height;
    GLFWwindow* glfw;
    callback render;
    struct vector key_handlers;
    struct mouse mouse;
};

static struct window Window = {0};

static inline void _framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    unused(window);
    Window.width = width;
    Window.height = height;
    Window.mouse.init = 0;
    glViewport(0, 0, width, height);
}

static inline int window_init(uint32_t width, uint32_t height) {
    Window.time = 0;
    Window.delta = 0;
    Window.width = width;
    Window.height = height;
    Window.glfw = nullptr;
    Window.render = nullptr;
    Window.mouse = (struct mouse){0};
    vec_init(&Window.key_handlers, sizeof(struct key_handler));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(Window.width, Window.height, "App", nullptr, nullptr);
    if (!window) {
        printf("window_init failed:\ncould not create window\n");
        return 0;
    }

    glfwMakeContextCurrent(window);

    if (!load_gl_procs((ProcLoader)glfwGetProcAddress)) {
        printf("window_init failed:\ncould not load OpenGL\n");
        return 0;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, _framebuffer_size_callback);

    Window.glfw = window;

    return 1;
}

static inline void window_set_clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

static inline void window_set_render_callback(callback cb) {
    Window.render = cb;
}

static inline void window_set_key_handler(int key, callback cb, uint32_t debounce) {
    struct key_handler* handler = vec_emplace(&Window.key_handlers);
    handler->debounce = (double)debounce / 1000.;
    handler->key = key;
    handler->handler = cb;
    handler->last_press = 0;
}

static inline void _key_handler_check_and_run(void* handler) {
    struct key_handler* p = handler;

    if (glfwGetKey(Window.glfw, p->key) == GLFW_PRESS) {
        double time = glfwGetTime();
        if (time > p->last_press + p->debounce) {
            p->last_press = time;
            p->handler();
        }
    }
}

static inline void _mouse_callback(GLFWwindow* window, double x, double y) {
    unused(window);

    if (!Window.mouse.init) {
        Window.mouse.lastx = x;
        Window.mouse.lasty = y;
        Window.mouse.init = 1;
    }

    float xdelta = x - Window.mouse.lastx;
    float ydelta = y - Window.mouse.lasty;

    if (Window.mouse.handler)
        Window.mouse.handler(x, y, xdelta, ydelta);

    Window.mouse.lastx = x;
    Window.mouse.lasty = y;
}

static inline void window_set_mouse_handler(mouse_callback cb) {
    Window.mouse.handler = cb;
    Window.mouse.init = 0;

    glfwSetCursorPosCallback(Window.glfw, _mouse_callback);
}

static inline void window_run() {
    while (!glfwWindowShouldClose(Window.glfw)) {
        if (glfwGetKey(Window.glfw, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(Window.glfw, true);

        float time = glfwGetTime();
        Window.delta = time - Window.time;
        Window.time = time;

        vec_for_each(&Window.key_handlers, _key_handler_check_and_run);

        if (Window.render)
            Window.render();

        glfwSwapBuffers(Window.glfw);
        glfwPollEvents();
    }
}

static inline float window_time() {
    return Window.time;
}

static inline float window_delta() {
    return Window.delta;
}

static inline void window_uninit() {
    if (Window.glfw)
        glfwDestroyWindow(Window.glfw);

    glfwTerminate();
}
