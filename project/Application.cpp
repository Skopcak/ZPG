#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Application.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

#include "Camera.h"
#include "InputController.h"
#include "ClockAnimation.h"
#include "SolarAnimation.h"
#include "RenderResources.h"
#include "SceneContent.h"
#include "SceneBuilder.h"
#include "SceneBuilders.h"
#include "SceneSelection.h"
#include "SceneLighting.h"
#include "TextureScenes.h"

static void error_callback(int error, const char* description){
    std::cerr << "GLFW error " << error << ": "
        << description << '\n';
}

static void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(
    GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

static bool updateViewportAndProjection(GLFWwindow* window, Camera& camera)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0)
        return false;
    glViewport(0, 0, width, height);
    camera.updateProjection(width, height);
    return true;
}

static float calculateDeltaTime(double& lastTime)
{
    const double currentTime = glfwGetTime();
    const float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    return deltaTime;
}

Application::Application(): window(nullptr){
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()){
        std::cerr << "GLFW initialization failed\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);

    if (!window){
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
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
    GLint maxTextureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Texture units (fragment shader): " << maxTextureUnits << '\n'
              << "F1: textured plane; F2: textured door; 0-9: previous scenes\n";

    Camera camera;
    RenderResources resources(camera);
    SceneContent content;
    SceneBuilder builder(resources, content);
    const auto basic = buildBasicScenes(builder);
    const auto tests = buildLightingTestScenes(builder);
    const auto forests = buildForestScenes(builder, basic.signature);
    const auto solar = buildSolarScene(builder, basic.signature);
    const auto clock = buildClockScene(builder, forests.day.scene);
    SolarAnimation solarAnimation(solar.rotations);
    TextureScenes textureScenes(camera);
    const SceneLighting lighting(resources, forests.night.controlledObject.getTransformation());
    const auto entries = createSceneEntries(basic, forests, solar, clock, tests, textureScenes, lighting);
    const SceneEntry* active = &entries.front();
    InputController input(window, camera);

    updateViewportAndProjection(window, camera);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)){
        const float deltaTime = calculateDeltaTime(lastTime);
        solarAnimation.update(deltaTime);
        selectScene(window, entries, active);
        input.update(*active->controlledObject, deltaTime);
        if (!updateViewportAndProjection(window, camera)){
            glfwPollEvents();
            continue;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        updateClockHands(resources.clockProgram);
        applySceneLighting(*active, resources);
        textureScenes.bindTexture(*active->scene);
        active->scene->draw();
        textureScenes.unbindTexture(*active->scene);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glUseProgram(0);
}

Application::~Application()
{
    // Finish deleting any bound program left pending during exception unwinding.
    glUseProgram(0);
    glfwDestroyWindow(window);
    glfwTerminate();
}
