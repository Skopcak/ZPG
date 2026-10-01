#include "Application.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

#include "ShaderProgram.h"
#include "Model.h"
#include "DrawableObject.h"
#include "Scene.h"
#include "Sphere.h"

//#include <sphere.h>
#include <OpenGL.h>

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
    ShaderProgram colorProgram(
        "shaders/basic.vert", "shaders/basic.frag");

    ShaderProgram yellowProgram(
        "shaders/yellow.vert", "shaders/yellow.frag");

    // Jeden model, ktory pouziju oba objekty.
    //const GLsizei sphereVertexCount = static_cast<GLsizei>(
    //    sizeof(sphere) / (6 * sizeof(float)));

    //Model sphereModel(sphere, sphereVertexCount);

    //DrawableObject colorSphere(sphereModel, colorProgram);
    //DrawableObject yellowSphere(sphereModel, yellowProgram);

    //// Scena obsahuje odkazy na oba objekty.
    //Scene scene;
    //scene.addObject(colorSphere);
    //scene.addObject(yellowSphere);
   
    const GLsizei logoVertexCount = static_cast<GLsizei>(
        sizeof(opengl) / (6 * sizeof(float)));

    Model logoModel(opengl, logoVertexCount);
    DrawableObject logoObject(logoModel, colorProgram);

    Scene scene;
    scene.addObject(logoObject);

    const GLsizei sphereVertexCount = static_cast<GLsizei>(
        sizeof(sphere) / (6 * sizeof(float)));

    Model sphereModel(sphere, sphereVertexCount);
    DrawableObject sphereObject(sphereModel, colorProgram);

    sphereObject.getTransformation().scaleFactor = 0.35f;

    Scene sphereScene;
    sphereScene.addObject(sphereObject);

    Scene* activeScene = &scene;

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(
        window, &framebufferWidth, &framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Zachovanie doterajsieho vzhladu gul.
    glEnable(GL_DEPTH_TEST);

    // Hodnoty uchovavame medzi jednotlivymi snimkami.
    float offsetX = -0.5f;
    float offsetY = 0.0f;
    float speed = 0.5f;
    double lastTime = glfwGetTime();
    float angle = 0.5f;
    float rotationSpeed = 1.0f;
    float scaleFactor = 0.35f;
    float scaleSpeed = 0.5f;
    
    while (!glfwWindowShouldClose(window))
    {
        // Cas od predchadzajuceho snimku.
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        //scene switch
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            activeScene = &scene;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            activeScene = &sphereScene;
        }

        // Pohyb pomocou sipok.
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            offsetX -= speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            offsetX += speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            offsetY += speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            offsetY -= speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            angle -= rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            angle += rotationSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            scaleFactor -= scaleSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            scaleFactor += scaleSpeed * deltaTime;
        }

        if (scaleFactor < 0.05f)
        {
            scaleFactor = 0.05f;
        }
     
        Transformation& transform = logoObject.getTransformation();

        transform.offsetX = offsetX;
        transform.offsetY = offsetY;
        transform.angle = angle;
        transform.scaleFactor = scaleFactor;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        activeScene->draw();

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