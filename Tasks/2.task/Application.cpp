#include "Application.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "ShaderProgram.h"
#include "Model.h"
#include "DrawableObject.h"
#include "Scene.h"
#include "Sphere.h"
#include "tree.h"
#include "bushes.h"
#include "Login.h"


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
        "shaders/basic.vert", "shaders/yellow.frag");

    // Create the login scene from the exported model.
    const GLsizei logoVertexCount = static_cast<GLsizei>(
        sizeof(loginVertices) / (6 * sizeof(float)));

    Model logoModel(loginVertices, logoVertexCount);
    DrawableObject logoObject(logoModel, colorProgram);

    Scene scene;
    scene.addObject(logoObject);

    // Create the sphere scene using the color shader.
    const GLsizei sphereVertexCount = static_cast<GLsizei>(
        sizeof(sphere) / (6 * sizeof(float)));

    Model sphereModel(sphere, sphereVertexCount);
    DrawableObject sphereObject(sphereModel, colorProgram);

    sphereObject.getTransformation().scaleFactor = 0.35f;

    Scene sphereScene;
    sphereScene.addObject(sphereObject);

    // Create a triangle with an RGB color for each vertex.
    const float triangleVertices[] = {
       
         0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f
    };

    const GLsizei triangleVertexCount = static_cast<GLsizei>(
        sizeof(triangleVertices) / (6 * sizeof(float)));

    Model triangleModel(triangleVertices, triangleVertexCount);
    DrawableObject triangleObject(triangleModel, colorProgram);

    Scene triangleScene;
    triangleScene.addObject(triangleObject);
   
    // Build the forest from instances of one tree model.
    const GLsizei treeVertexCount = static_cast<GLsizei>(
        sizeof(tree) / (6 * sizeof(float)));

    Model treeModel(tree, treeVertexCount);
   
    const int treeCount = 12;
    std::vector<DrawableObject> trees;
    // Reserve space to keep object addresses valid while adding trees.
    trees.reserve(treeCount);

    Scene forestScene;

    for (int i = 0; i < treeCount; ++i)
    {
        trees.emplace_back(treeModel, colorProgram);

        DrawableObject& treeObject = trees.back();
        Transformation& t = treeObject.getTransformation();

        int column = i % 4;
        int row = i / 4;

        t.offsetX = -0.72f + 0.48f * column;
        t.offsetY = -0.85f + 0.50f * row;
        t.scaleFactor = 0.05f + 0.005f * (i % 3);
        t.angle = 0.2f * column;

        forestScene.addObject(treeObject);
    }

    // Place smaller bushes between the trees.
    const GLsizei bushVertexCount = static_cast<GLsizei>(
        sizeof(bushes) / (6 * sizeof(float)));

    Model bushModel(bushes, bushVertexCount);

    const int bushCount = 12;
    std::vector<DrawableObject> bushObjects;
    bushObjects.reserve(bushCount);

    for (int i = 0; i < bushCount; ++i)
    {
        bushObjects.emplace_back(bushModel, colorProgram);

        DrawableObject& bushObject = bushObjects.back();
        Transformation& t = bushObject.getTransformation();

        int column = i % 4;
        int row = i / 4;

        t.offsetX = -0.56f + 0.48f * column;
        t.offsetY = -0.83f + 0.50f * row;
        t.offsetZ = -0.2f;
        t.scaleFactor = 0.25f;

        forestScene.addObject(bushObject);
    }

    // Reuse the sphere model with a yellow shader for the sun.
    DrawableObject sunObject(sphereModel, yellowProgram);

    Transformation& sunTransform = sunObject.getTransformation();
    sunTransform.offsetX = 0.75f;
    sunTransform.offsetY = 0.78f;
    sunTransform.offsetZ = 0.5f;
    sunTransform.scaleFactor = 0.12f;

    forestScene.addObject(sunObject);

    // Share a small login signature across the other scenes.
    DrawableObject signatureObject(logoModel, yellowProgram);

    Transformation& signatureTransform =
        signatureObject.getTransformation();

    signatureTransform.offsetX = 0.70f;
    signatureTransform.offsetY = -0.90f;
    signatureTransform.offsetZ = -0.80f;
    signatureTransform.scaleFactor = 0.20f;
    signatureTransform.angle = 0.0f;

    sphereScene.addObject(signatureObject);
    triangleScene.addObject(signatureObject);
    forestScene.addObject(signatureObject);

    Scene* activeScene = &scene;
    DrawableObject* activeObject = &logoObject;

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(
        window, &framebufferWidth, &framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);

    Transformation& logoTransform = logoObject.getTransformation();
    
    logoTransform.offsetX = 0.0f;
    logoTransform.offsetY = 0.0f;
    logoTransform.angle = 0.5f;
    logoTransform.scaleFactor = 0.8f;

    float speed = 0.5f;
    float rotationSpeed = 1.0f;
    float scaleSpeed = 0.5f;
    double lastTime = glfwGetTime();
    
    while (!glfwWindowShouldClose(window))
    {
        // Use elapsed time to keep movement independent of frame rate.
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        // Select the scene and the object controlled by the keyboard.
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            activeScene = &scene;
            activeObject = &logoObject;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            activeScene = &sphereScene;
            activeObject = &sphereObject;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            activeScene = &triangleScene;
            activeObject = &triangleObject;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            activeScene = &forestScene;
            activeObject = &trees.front();
        }

        Transformation& transform = activeObject->getTransformation();

        float& offsetX = transform.offsetX;
        float& offsetY = transform.offsetY;
        float& angle = transform.angle;
        float& scaleFactor = transform.scaleFactor;


        // Move, rotate and scale the selected object.
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
        
        // Render the selected scene and display the frame.
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