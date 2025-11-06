#include "glad/glad.h"

#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "shaders.h"
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

GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        printf("shader compilation failed:\n%s\n", infoLog);
    }

    return shader;
}

static GLuint shaderProgram;

void compileShaderProgram() {
    GLuint vertexShader = compileShader((char*)&vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader((char*)fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        printf("program linking failed:\n%s\n", infoLog);
    }

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

static GLuint VAO;

void initializeVertexData() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void drawFrame() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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

    compileShaderProgram();
    initializeVertexData();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        drawFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

terminate:
    glfwTerminate();

    return ret;
}
