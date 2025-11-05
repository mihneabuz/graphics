#include "glad/glad.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "util.h"

#define err_out(msg, label)         \
    {                               \
        printf("error: " msg "\n"); \
        ret = -1;                   \
        goto label;                 \
    }

static int width = 800;
static int height = 600;

void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
    unused(window);
    width = w;
    height = h;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    int ret = 0;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "App", nullptr, nullptr);
    if (!window)
        err_out("failed to create window", terminate);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        err_out("failed to load GL functions", terminate);

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

terminate:
    glfwTerminate();

    return ret;
}
