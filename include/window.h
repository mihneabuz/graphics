#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

#include "util.h"
#include "vector.h"

struct key_handler {
    callback handler;
    int key;
    double debounce;
    double last_press;
};

struct window {
    GLFWwindow* glfw;
    uint32_t width;
    uint32_t height;
    callback render;
    struct vector key_handlers;
};

static struct window Window = {0};

static inline void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    unused(window);
    Window.width = width;
    Window.height = height;
    glViewport(0, 0, width, height);
}

static inline int window_init(uint32_t width, uint32_t height) {
    Window.glfw = nullptr;
    Window.width = width;
    Window.height = height;
    Window.render = nullptr;
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("window_init failed:\ncould not load OpenGL\n");
        return 0;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

static inline void key_handler_check_and_run(void* handler) {
    struct key_handler* p = handler;

    if (glfwGetKey(Window.glfw, p->key) == GLFW_PRESS) {
        double time = glfwGetTime();
        if (time > p->last_press + p->debounce) {
            p->last_press = time;
            p->handler();
        }
    }
}

static inline void window_run() {
    while (!glfwWindowShouldClose(Window.glfw)) {
        if (glfwGetKey(Window.glfw, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(Window.glfw, true);

        vec_for_each(&Window.key_handlers, key_handler_check_and_run);

        if (Window.render)
            Window.render();

        glfwSwapBuffers(Window.glfw);
        glfwPollEvents();
    }
}

static inline void window_uninit() {
    if (Window.glfw)
        glfwDestroyWindow(Window.glfw);

    glfwTerminate();
}
