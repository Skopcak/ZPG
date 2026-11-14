#include "InputController.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "DrawableObject.h"

InputController::InputController(GLFWwindow* window, Camera& camera)
    : window(window), camera(camera)
{
    glfwGetCursorPos(window, &previousMouseX, &previousMouseY);
}

void InputController::update(DrawableObject* activeObject, float deltaTime)
{
    if (activeObject != nullptr) {
        updateObject(*activeObject, deltaTime);
    }
    updateCamera(deltaTime);
}

void InputController::updateObject(DrawableObject& object, float deltaTime) const
{
    constexpr float movementSpeed = 0.5f;
    constexpr float rotationSpeed = 1.0f;
    constexpr float scaleSpeed = 0.5f;

    Transformation& transform = object.getTransformation();

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        transform.offsetX -= movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        transform.offsetX += movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        transform.offsetY += movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        transform.offsetY -= movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        transform.angle -= rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        transform.angle += rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        transform.scaleFactor -= scaleSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        transform.scaleFactor += scaleSpeed * deltaTime;
    }

    if (transform.scaleFactor < 0.05f) {
        transform.scaleFactor = 0.05f;
    }
}

void InputController::updateCamera(float deltaTime)
{
    constexpr float cameraSpeed = 2.0f;
    constexpr float mouseSensitivity = 0.003f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.moveForward(cameraSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveForward(-cameraSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.moveRight(cameraSpeed * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.moveRight(-cameraSpeed * deltaTime);
    }

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    const bool looking =
        glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE &&
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (looking && wasLooking) {
        const float deltaAlpha =
            static_cast<float>(mouseY - previousMouseY) * mouseSensitivity;
        const float deltaFi =
            static_cast<float>(mouseX - previousMouseX) * mouseSensitivity;

        camera.rotate(deltaAlpha, deltaFi);
    }

    previousMouseX = mouseX;
    previousMouseY = mouseY;
    wasLooking = looking;
}
