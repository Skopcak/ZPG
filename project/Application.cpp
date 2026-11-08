#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Application.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <chrono>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#include <glm/trigonometric.hpp>

#include "Camera.h"
#include "InputController.h"
#include "RenderResources.h"
#include "DrawableObject.h"
#include "Scene.h"
#include "SceneFactory.h"
#include "Light.h"
#include "Material.h"
#include "Translation.h"
#include "Rotation.h"
#include "Scale.h"
#include "CompositeTransformation.h"
#include "Texture.h"
#include "SkyRenderer.h"

#include "Models/fence/plain_uv.h"
#include "Models/door/door.h"

static void error_callback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(
    GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static bool updateViewportAndProjection(GLFWwindow* window, Camera& camera) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0)
        return false;
    glViewport(0, 0, width, height);
    camera.updateProjection(width, height);
    return true;
}

static float calculateDeltaTime(double& lastTime) {
    const double currentTime = glfwGetTime();
    const float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    return deltaTime;
}

static void updateClockHands(
    const ShaderProgram& program,
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now()) {
    const auto wholeSeconds = std::chrono::floor<std::chrono::seconds>(now);
    const std::time_t timestamp = std::chrono::system_clock::to_time_t(wholeSeconds);
    std::tm localTime{};
    if (localtime_s(&localTime, &timestamp) != 0) {
        std::cerr << "Unable to read local time\n";
        std::exit(EXIT_FAILURE);
    }

    const float seconds = static_cast<float>(localTime.tm_sec) + std::chrono::duration<float>(now - wholeSeconds).count();
    const float minutes = static_cast<float>(localTime.tm_min) + seconds / 60.0f;
    const float hours = static_cast<float>(localTime.tm_hour % 12) + minutes / 60.0f;

    const float clockwiseTurn = glm::radians(-360.0f);

    const Rotation hourHandRotation(clockwiseTurn * (hours / 12.0f), 0.0f, 0.0f, 1.0f);
    const Rotation minuteHandRotation(
        clockwiseTurn * (minutes / 60.0f), 0.0f, 0.0f, 1.0f
    );
    const Rotation secondHandRotation(
        clockwiseTurn * (seconds / 60.0f), 0.0f, 0.0f, 1.0f
    );

    program.setUniform("hourMatrix", hourHandRotation.getMatrix());
    program.setUniform("minuteMatrix", minuteHandRotation.getMatrix());
    program.setUniform("secondMatrix", secondHandRotation.getMatrix());
}

Application::Application() : window(nullptr) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void Application::run() {
    GLint maxTextureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Texture units (fragment shader): " << maxTextureUnits << '\n'
              << "F1: wood; F2: door; F3: two textures; F4: two models\n"
              << "F5: EB408 classroom with skybox; F6: forest with skydome; 0-9: previous scenes\n";

    Camera camera;
    RenderResources resources(camera);

    // Screen-space login, shared by every scene.
    ShaderProgram loginProgram("login.vert", "login.frag");
    const float loginVertices[] = {
        0,0,0, 0,0,1, 0,0,   1,0,0, 0,0,1, 1,0,   1,1,0, 0,0,1, 1,1,
        0,0,0, 0,0,1, 0,0,   1,1,0, 0,0,1, 1,1,   0,1,0, 0,0,1, 0,1
    };
    Model loginModel(loginVertices, 6, 8);
    Texture loginTexture("Textures/login.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    DrawableObject loginOverlay(loginModel, loginProgram);
    loginOverlay.setTexture(loginTexture);
    loginProgram.setUniform("textureUnitID", 0);

    // Login (1)
    DrawableObject logoObject(resources.logoModel, resources.colorProgram);
    logoObject.getTransformation().angle = 0.5f;
    logoObject.getTransformation().scaleFactor = 0.8f;
    Scene loginScene;
    loginScene.addObject(logoObject);

    // Sphere (2)
    Scene sphereScene;
    SceneFactory sphereFactory(sphereScene);
    DrawableObject& sphereObject = sphereFactory.addObject(resources.sphereModel, resources.blinnProgram);
    sphereObject.getTransformation().scaleFactor = 0.35f;

    // Triangle (3)
    Translation triangleTranslation(0.4f, 0.0f, 0.0f);
    Rotation triangleRotation(0.7854f, 0.0f, 0.0f, 1.0f);
    Scale triangleScale(0.6f, 0.6f, 0.6f);
    CompositeTransformation triangleMovement;
    triangleMovement.add(triangleTranslation);
    triangleMovement.add(triangleRotation);
    CompositeTransformation triangleTransformation;
    triangleTransformation.add(triangleMovement);
    triangleTransformation.add(triangleScale);
    DrawableObject triangleObject(resources.triangleModel, resources.colorProgram);
    triangleObject.setTransformation(triangleTransformation);
    Scene triangleScene;
    triangleScene.addObject(triangleObject);

    // Shared signature
    DrawableObject signatureObject(resources.logoModel, resources.yellowProgram);
    Transformation& signature = signatureObject.getTransformation();
    signature.offsetX = 0.70f;
    signature.offsetY = -0.90f;
    signature.offsetZ = -0.80f;
    signature.scaleFactor = 0.20f;
    sphereScene.addObject(signatureObject);
    triangleScene.addObject(signatureObject);

    // Lambert lighting test (7)
    const glm::vec3 lightingPositions[] = {
        {0.0f, 0.8f, 0.0f},
        {-0.8f, 0.0f, 0.0f},
        {0.8f, 0.0f, 0.0f},
        {0.0f, -0.8f, 0.0f}
    };
    std::vector<DrawableObject> lightingSpheres;
    lightingSpheres.reserve(4);
    Scene lightingScene;
    for (const auto& position : lightingPositions) {
        lightingSpheres.emplace_back(resources.sphereModel, resources.lambertProgram);
        DrawableObject& object = lightingSpheres.back();
        Transformation& transform = object.getTransformation();
        transform.offsetX = position.x;
        transform.offsetY = position.y;
        transform.offsetZ = position.z;
        transform.scaleFactor = 0.35f;
        lightingScene.addObject(object);
    }

    // Normal matrix test (8)
    Scale normalTestScale(1.5f, 0.5f, 1.0f);
    DrawableObject normalTestObject(resources.sphereModel, resources.normalTestProgram);
    normalTestObject.getTransformation().scaleFactor = 0.7f;
    normalTestObject.setTransformation(normalTestScale);
    Scene normalTestScene;
    normalTestScene.addObject(normalTestObject);
    resources.normalTestProgram.setUniform("normalMode", 2.0f);

    // Specular test (9)
    DrawableObject specularTestObject(
        resources.sphereModel,
        resources.specularTestProgram
    );
    specularTestObject.getTransformation().scaleFactor = 0.7f;
    Scene specularTestScene;
    specularTestScene.addObject(specularTestObject);
    resources.specularTestProgram.setUniform("shininess", 1.0f);
    resources.specularTestProgram.setUniform("clipBackfaceSpecular", 1.0f);

    // Forest (4)
    DrawableObject groundObject(resources.groundModel, resources.colorProgram);
    groundObject.getTransformation().scaleFactor = 10.0f;
    groundObject.getTransformation().offsetY = -1.0f;
    Scene forestScene;
    forestScene.addObject(groundObject);

    std::mt19937 forestGenerator(std::random_device{}());
    std::uniform_real_distribution<float> forestX(-8.0f, 8.0f);
    std::uniform_real_distribution<float> forestZ(-8.0f, 8.0f);
    std::uniform_real_distribution<float> forestAngle(0.0f, 6.283185f);
    std::uniform_real_distribution<float> treeScaleRange(0.25f, 0.45f);
    std::uniform_real_distribution<float> bushScaleRange(1.0f, 2.0f);

    // Trees. Reserve keeps the object addresses stored by Scene valid.
    const int treeCount = 74;
    std::vector<DrawableObject> trees;
    trees.reserve(treeCount);
    for (int i = 0; i < treeCount; ++i) {
        trees.emplace_back(resources.treeModel, resources.treeProgram);
        DrawableObject& tree = trees.back();
        Transformation& transform = tree.getTransformation();
        transform.offsetX = forestX(forestGenerator);
        transform.offsetY = -1.0f;
        transform.offsetZ = forestZ(forestGenerator);
        transform.scaleFactor = treeScaleRange(forestGenerator);
        transform.angle = forestAngle(forestGenerator);
        forestScene.addObject(tree);
    }

    const int bushCount = 25;
    std::vector<DrawableObject> bushes;
    bushes.reserve(bushCount);
    for (int i = 0; i < bushCount; ++i) {
        bushes.emplace_back(resources.bushModel, resources.bushProgram);
        DrawableObject& bush = bushes.back();
        Transformation& transform = bush.getTransformation();
        transform.offsetX = forestX(forestGenerator);
        transform.offsetY = -1.0f;
        transform.offsetZ = forestZ(forestGenerator);
        transform.scaleFactor = bushScaleRange(forestGenerator);
        transform.angle = forestAngle(forestGenerator);
        forestScene.addObject(bush);
    }

    // Night forest (0)
    // Copy daytime placement while sharing the same GPU meshes.
    DrawableObject nightGroundObject(resources.groundModel, resources.nightGroundProgram);
    nightGroundObject.getTransformation() = groundObject.getTransformation();
    Scene nightForestScene;
    nightForestScene.addObject(nightGroundObject);

    std::vector<DrawableObject> nightTrees;
    nightTrees.reserve(treeCount);
    for (int i = 0; i < treeCount; ++i) {
        nightTrees.emplace_back(resources.treeModel, resources.nightTreeProgram);
        DrawableObject& tree = nightTrees.back();
        tree.getTransformation() = trees[i].getTransformation();
        Transformation& transform = tree.getTransformation();

        // Leave a clearing between the initial camera and the firefly.
        if (transform.offsetX > -2.0f && transform.offsetX < 2.0f
            && transform.offsetZ > -2.0f && transform.offsetZ < 5.0f) {
            transform.offsetX += transform.offsetX < 0.0f ? -4.0f : 4.0f;
        }
        nightForestScene.addObject(tree);
    }

    std::vector<DrawableObject> nightBushes;
    nightBushes.reserve(bushCount);
    for (int i = 0; i < bushCount; ++i) {
        nightBushes.emplace_back(resources.bushModel, resources.nightBushProgram);
        DrawableObject& bush = nightBushes.back();
        bush.getTransformation() = bushes[i].getTransformation();
        Transformation& transform = bush.getTransformation();
        if (transform.offsetX > -2.0f && transform.offsetX < 2.0f
            && transform.offsetZ > -2.0f && transform.offsetZ < 5.0f) {
            transform.offsetX += transform.offsetX < 0.0f ? -4.0f : 4.0f;
        }
        nightForestScene.addObject(bush);
    }

    const Material vegetation{.useVertexColor = true};
    const Material soil{.color = {0.18f, 0.12f, 0.06f}};
    vegetation.apply(resources.nightTreeProgram);
    vegetation.apply(resources.nightBushProgram);
    soil.apply(resources.nightGroundProgram);

    // Firefly: the visible marker for the night light.
    DrawableObject fireflyObject(resources.sphereModel, resources.yellowProgram);
    Transformation& fireflyTransform = fireflyObject.getTransformation();
    fireflyTransform.offsetY = -0.3f;
    fireflyTransform.scaleFactor = 0.05f;
    nightForestScene.addObject(fireflyObject);

    DrawableObject sunObject(resources.sphereModel, resources.yellowProgram);
    Transformation& sunTransform = sunObject.getTransformation();
    sunTransform.offsetX = 0.75f;
    sunTransform.offsetY = 0.78f;
    sunTransform.offsetZ = 0.5f;
    sunTransform.scaleFactor = 0.12f;
    forestScene.addObject(sunObject);
    forestScene.addObject(signatureObject);

    // Solar system (5)
    DrawableObject solarSunObject(resources.solarSunModel, resources.ambientProgram);
    DrawableObject earthObject(resources.earthModel, resources.phongProgram);
    DrawableObject moonObject(resources.moonModel, resources.blinnProgram);
    DrawableObject venatorObject(resources.venatorModel, resources.phongProgram);
    DrawableObject imperialObject(resources.imperialModel, resources.blinnProgram);
    Rotation solarSunSpin(0.0f, 0.0f, 1.0f, 0.0f);
    Scale solarSunScale(0.18f, 0.18f, 0.18f);
    CompositeTransformation solarSunTransformation;
    solarSunTransformation.add(solarSunSpin);
    solarSunTransformation.add(solarSunScale);
    solarSunObject.setTransformation(solarSunTransformation);

    // Earth: orbit first, then local rotation and size.
    Translation earthDistance(0.55f, 0.0f, 0.0f);
    Rotation earthOrbitRotation(0.0f, 0.0f, 0.0f, 1.0f);
    CompositeTransformation earthOrbit;
    earthOrbit.add(earthOrbitRotation);
    earthOrbit.add(earthDistance);

    Rotation earthSpin(0.0f, 1.0f, 0.0f, 0.0f);
    Scale earthScale(0.10f, 0.10f, 0.10f);

    CompositeTransformation earthTransformation;
    earthTransformation.add(solarSunObject.getTransformation());
    earthTransformation.add(earthOrbit);
    earthTransformation.add(earthSpin);
    earthTransformation.add(earthScale);
    earthObject.setTransformation(earthTransformation);

    // Moon: follow Earth's orbital position, independently of Earth's scale.
    Translation moonDistance(0.20f, 0.0f, 0.0f);
    Rotation moonOrbitRotation(0.0f, 0.0f, 0.0f, 1.0f);
    CompositeTransformation moonOrbit;
    moonOrbit.add(moonOrbitRotation);
    moonOrbit.add(moonDistance);

    Rotation moonSpin(0.0f, 1.0f, 0.0f, 0.0f);
    Scale moonScale(0.045f, 0.045f, 0.045f);

    CompositeTransformation moonTransformation;
    moonTransformation.add(solarSunObject.getTransformation());
    moonTransformation.add(earthOrbit);
    moonTransformation.add(moonOrbit);
    moonTransformation.add(moonSpin);
    moonTransformation.add(moonScale);
    moonObject.setTransformation(moonTransformation);

    // Ships: orbital rotation, distance, local tilt and size.
    Rotation venatorOrbitRotation(0.0f, 0.0f, 1.0f, 0.0f);
    Rotation imperialOrbitRotation(0.0f, 0.0f, 1.0f, 0.0f);
    Translation venatorDistance(0.0f, 0.0f, 1.05f);
    Translation imperialDistance(0.0f, 0.0f, 1.40f);
    Rotation imperialFacing(glm::radians(180.0f), 0.0f, 0.0f, 1.0f);
    Rotation shipTilt(glm::radians(65.0f), 1.0f, 0.0f, 0.0f);
    Scale shipScale(0.28f, 0.28f, 0.28f);
    CompositeTransformation venatorTransformation;
    venatorTransformation.add(solarSunObject.getTransformation());
    venatorTransformation.add(venatorOrbitRotation);
    venatorTransformation.add(venatorDistance);
    venatorTransformation.add(shipTilt);
    venatorTransformation.add(shipScale);
    venatorObject.setTransformation(venatorTransformation);

    CompositeTransformation imperialTransformation;
    imperialTransformation.add(solarSunObject.getTransformation());
    imperialTransformation.add(imperialOrbitRotation);
    imperialTransformation.add(imperialDistance);
    imperialTransformation.add(imperialFacing);
    imperialTransformation.add(shipTilt);
    imperialTransformation.add(shipScale);
    imperialObject.setTransformation(imperialTransformation);

    Scene solarScene;
    solarScene.addObject(solarSunObject);
    solarScene.addObject(earthObject);
    solarScene.addObject(moonObject);
    solarScene.addObject(venatorObject);
    solarScene.addObject(imperialObject);
    solarScene.addObject(signatureObject);

    // Clocks (6)
    // All three clocks share the same face transformation and shader program.
    Scale clockFacing(1.0f, 1.0f, -1.0f);
    DrawableObject clockObject(resources.clockMesh, resources.clockProgram);
    clockObject.setTransformation(clockFacing);
    Scene clockScene;
    clockScene.addObject(clockObject);

    DrawableObject firstForestClock(resources.clockMesh, resources.clockProgram);
    firstForestClock.setTransformation(clockFacing);
    firstForestClock.getTransformation().offsetX = -1.5f;
    firstForestClock.getTransformation().offsetY = -0.25f;
    firstForestClock.getTransformation().scaleFactor = 0.75f;
    forestScene.addObject(firstForestClock);

    DrawableObject secondForestClock(resources.clockMesh, resources.clockProgram);
    secondForestClock.setTransformation(clockFacing);
    secondForestClock.getTransformation().offsetX = 1.5f;
    secondForestClock.getTransformation().offsetY = -0.25f;
    secondForestClock.getTransformation().offsetZ = -3.0f;
    secondForestClock.getTransformation().scaleFactor = 0.75f;
    forestScene.addObject(secondForestClock);

    // Textured models (F1-F4)
    Texture fenceTexture("Models/fence/wooden_fence.png");
    Texture doorTexture("Models/door/T_door_basecolor.png");
    Texture detailTexture("Textures/detail.png");

    ShaderProgram textureProgram("texture.vert", "texture.frag");
    ShaderProgram multiTextureProgram("texture.vert", "multi_texture.frag");
    textureProgram.setUniform("textureUnitID", 0);
    multiTextureProgram.setUniform("baseTexture", 0);
    multiTextureProgram.setUniform("detailTexture", 1);
    multiTextureProgram.setUniform("detailStrength", 0.4f);

    // Geometry: position, normal and UV coordinates (8 floats per vertex).
    Model planeModel(
        plainUV,
        static_cast<GLsizei>(sizeof(plainUV) / (8 * sizeof(float))),
        8
    );
    Model doorModel(doorVertices, doorVertexCount, 8);

    // Face the plane toward the camera and preserve the image aspect ratio.
    Scale planeAspect(
        static_cast<float>(fenceTexture.getWidth()) / fenceTexture.getHeight(),
        1.0f,
        1.0f
    );
    Rotation planeOrientation(glm::radians(-90.0f), 0.0f, 0.0f, 1.0f);
    Rotation planeFacing(glm::radians(90.0f), 1.0f, 0.0f, 0.0f);
    CompositeTransformation planeTransformation;
    planeTransformation.add(planeAspect);
    planeTransformation.add(planeOrientation);
    planeTransformation.add(planeFacing);
    DrawableObject planeObject(planeModel, textureProgram);
    DrawableObject doorObject(doorModel, textureProgram);
    DrawableObject multiTextureObject(planeModel, multiTextureProgram);
    DrawableObject comparisonPlane(planeModel, textureProgram);
    DrawableObject comparisonDoor(doorModel, textureProgram);
    for (DrawableObject* plane : {&planeObject, &multiTextureObject, &comparisonPlane}) {
        plane->setTransformation(planeTransformation);
        plane->setTexture(fenceTexture);
    }

    // F1: Wooden plane.
    planeObject.getTransformation().scaleFactor = 2.0f;

    // F2: Door.
    doorObject.setTexture(doorTexture);
    doorObject.getTransformation().scaleFactor = 2.2f;
    doorObject.getTransformation().angle = glm::radians(165.0f);

    // F3: Two textures on one plane.
    multiTextureObject.getTransformation().scaleFactor = 2.0f;
    multiTextureObject.setTexture(detailTexture, 1);

    // F4: Two models, each with its own image.
    comparisonPlane.getTransformation().offsetX = -1.8f;
    comparisonPlane.getTransformation().scaleFactor = 1.1f;
    comparisonDoor.setTexture(doorTexture);
    comparisonDoor.getTransformation().offsetX = 1.6f;
    comparisonDoor.getTransformation().scaleFactor = 1.45f;
    comparisonDoor.getTransformation().angle = glm::radians(165.0f);

    Scene planeScene;
    planeScene.addObject(planeObject);

    Scene doorScene;
    doorScene.addObject(doorObject);

    Scene multiTextureScene;
    multiTextureScene.addObject(multiTextureObject);

    Scene comparisonScene;
    comparisonScene.addObject(comparisonPlane);
    comparisonScene.addObject(comparisonDoor);

    camera.addObserver(textureProgram);
    camera.addObserver(multiTextureProgram);

    // Imported classroom (F5); keep the shared camera at its current position.
    ShaderProgram importedProgram("imported.vert", "imported.frag");
    Scene classroomScene;
    SceneFactory classroomFactory(classroomScene);
    DrawableObject& classroomObject = classroomFactory.addModel("Models/EB408/EB408.gltf", importedProgram);
    classroomObject.getTransformation().offsetY = -1.6f;
    DrawableObject& classroomFurniture = classroomFactory.addModel("Models/EB408/Furniture.gltf", importedProgram);
    classroomFurniture.setTransformation(classroomObject.getTransformation());
    camera.addObserver(importedProgram);

    // Shared skybox (F5) and forest skydome (F6)
    SkyRenderer sky(camera);
    SkyRenderer::Mode skyMode = SkyRenderer::Mode::None;

    // Lights
    const Light defaultLight{.position = {10.0f, 10.0f, 10.0f}};
    const Light lightingTestLight;
    const Light specularTestLight{.position = {0.0f, 0.0f, -3.0f}};
    const Light classroomLight{.position = {0.0f, 1.1f, 3.8f}};
    Light solarLight;
    Light nightLight{
        .position = {
            fireflyTransform.offsetX,
            fireflyTransform.offsetY,
            fireflyTransform.offsetZ
        },
        .color = {2.0f, 1.8f, 0.8f},
        .attenuationFactors = {1.0f, 0.7f, 1.8f}
    };
    nightLight.apply(resources.nightTreeProgram);
    nightLight.apply(resources.nightBushProgram);
    nightLight.apply(resources.nightGroundProgram);

    const ShaderProgram* lightingPrograms[] = {
        &resources.lambertProgram, &resources.phongProgram, &resources.blinnProgram,
        &resources.normalTestProgram, &resources.specularTestProgram,
        &resources.nightTreeProgram, &resources.nightBushProgram,
        &resources.nightGroundProgram
    };

    Scene* activeScene = &loginScene;
    DrawableObject* activeObject = &logoObject;
    InputController input(window, camera);

    float earthOrbitAngle = 0.0f;
    float moonOrbitAngle = 0.0f;
    float earthSpinAngle = 0.0f;
    float moonSpinAngle = 0.0f;
    float sunSpinAngle = 0.0f;
    float venatorOrbitAngle = glm::radians(-45.0f);
    float imperialOrbitAngle = glm::radians(45.0f);

    updateViewportAndProjection(window, camera);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const float deltaTime = calculateDeltaTime(lastTime);

        // Keep the solar system moving even while another scene is selected.
        earthOrbitAngle += 0.2f * deltaTime;
        moonOrbitAngle += 1.2f * deltaTime;
        earthOrbitRotation.setAngle(earthOrbitAngle);
        moonOrbitRotation.setAngle(moonOrbitAngle);

        constexpr float shipOrbitSpeed = 0.35f;
        venatorOrbitAngle += shipOrbitSpeed * deltaTime;
        imperialOrbitAngle -= shipOrbitSpeed * deltaTime;
        venatorOrbitRotation.setAngle(venatorOrbitAngle);
        imperialOrbitRotation.setAngle(imperialOrbitAngle);

        earthSpinAngle += 0.8f * deltaTime;
        moonSpinAngle += 0.6f * deltaTime;
        earthSpin.setAngle(earthSpinAngle);
        moonSpin.setAngle(moonSpinAngle);

        sunSpinAngle += 0.4f * deltaTime;
        solarSunSpin.setAngle(sunSpinAngle);

        // Independent checks preserve the existing priority when keys are held together.
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            activeScene = &loginScene;
            activeObject = &logoObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            activeScene = &sphereScene;
            activeObject = &sphereObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            activeScene = &triangleScene;
            activeObject = &triangleObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            activeScene = &forestScene;
            activeObject = &trees.front();
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            activeScene = &solarScene;
            activeObject = &solarSunObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            activeScene = &clockScene;
            activeObject = &clockObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            activeScene = &lightingScene;
            activeObject = &lightingSpheres.front();
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
            activeScene = &normalTestScene;
            activeObject = &normalTestObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
            activeScene = &specularTestScene;
            activeObject = &specularTestObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            activeScene = &nightForestScene;
            activeObject = &fireflyObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
            activeScene = &planeScene;
            activeObject = &planeObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
            activeScene = &doorScene;
            activeObject = &doorObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
            activeScene = &multiTextureScene;
            activeObject = &multiTextureObject;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {
            activeScene = &comparisonScene;
            activeObject = &comparisonPlane;
            skyMode = SkyRenderer::Mode::None;
        }

        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
            activeScene = &classroomScene;
            activeObject = &classroomObject;
            skyMode = SkyRenderer::Mode::Cube;
        }

        if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
            activeScene = &forestScene;
            activeObject = &trees.front();
            skyMode = SkyRenderer::Mode::Dome;
        }

        input.update(*activeObject, deltaTime);
        if (!updateViewportAndProjection(window, camera)) {
            glfwPollEvents();
            continue;
        }

        // Draw the background before the scene objects.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sky.draw(skyMode);
        updateClockHands(resources.clockProgram);

        // Select the light after the controlled object has moved.
        const Light* activeLight = &defaultLight;
        if (activeScene == &lightingScene) {
            activeLight = &lightingTestLight;
        } else if (activeScene == &solarScene) {
            const Transformation& transform = solarSunObject.getTransformation();
            solarLight.position = {
                transform.offsetX,
                transform.offsetY,
                transform.offsetZ
            };
            activeLight = &solarLight;
        } else if (activeScene == &specularTestScene) {
            activeLight = &specularTestLight;
        } else if (activeScene == &nightForestScene) {
            nightLight.position = {
                fireflyTransform.offsetX,
                fireflyTransform.offsetY,
                fireflyTransform.offsetZ
            };
            activeLight = &nightLight;
        }
        for (const ShaderProgram* program : lightingPrograms) {
            activeLight->applyPosition(*program);
        }

        const float useVertexColor = activeScene == &solarScene ? 1.0f : 0.0f;
        resources.ambientProgram.setUniform("useVertexColor", useVertexColor);
        resources.phongProgram.setUniform("useVertexColor", useVertexColor);
        resources.blinnProgram.setUniform("useVertexColor", useVertexColor);

        classroomLight.apply(importedProgram);
        activeScene->draw(camera.getCamera());

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        loginProgram.setUniform("viewportSize", static_cast<float>(width), static_cast<float>(height), 0.0f);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        loginOverlay.draw();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glUseProgram(0);
    camera.removeObserver(textureProgram);
    camera.removeObserver(multiTextureProgram);
    camera.removeObserver(importedProgram);
}

Application::~Application() {
    glUseProgram(0);
    glfwDestroyWindow(window);
    glfwTerminate();
}
