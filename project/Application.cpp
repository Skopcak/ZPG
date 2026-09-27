#include "Application.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

#include "ShaderProgram.h"
#include "Model.h"
#include "DrawableObject.h"
#include "Scene.h"

#include <sphere.h>

static void error_callback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": "
        << description << '\n';
}

static void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(
    GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Application::Application()
    : window(nullptr)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "GLAD initialization failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void Application::run()
{
    // Shader programy.
    ShaderProgram colorProgram(
        "shaders/basic.vert", "shaders/basic.frag");

    ShaderProgram yellowProgram(
        "shaders/yellow.vert", "shaders/yellow.frag");

    // Jeden model, ktory pouziju oba objekty.
    const GLsizei sphereVertexCount = static_cast<GLsizei>(
        sizeof(sphere) / (6 * sizeof(float)));

    Model sphereModel(sphere, sphereVertexCount);

    DrawableObject colorSphere(sphereModel, colorProgram);
    DrawableObject yellowSphere(sphereModel, yellowProgram);

    // Scena obsahuje odkazy na oba objekty.
    Scene scene;
    scene.addObject(colorSphere);
    scene.addObject(yellowSphere);

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(
        window, &framebufferWidth, &framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Zachovanie doterajsieho vzhladu gul.
    glDisable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glUseProgram(0);
}

Application::~Application()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}